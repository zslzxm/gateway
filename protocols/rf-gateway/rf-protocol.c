#include "rf-protocol.h"
#include "common.h"

#define RF_DEBUG

static void calc_cs(void *buf, int len)
{
    unsigned char *p = (unsigned char *)buf;
    unsigned char cs = 0;
    proto_tail_t *pCs = (proto_tail_t *)(p + len - sizeof(proto_tail_t));

    for (int i = 0; i < (len - (int)sizeof(proto_tail_t)); i++)
        cs += p[i];
    pCs->cs = 0x100 - cs;
}

static int check_cs(void *buf, int len)
{
    unsigned char cs = 0;
    unsigned char *p = (unsigned char *)buf;
    for (int i = 0; i < len; i++)
        cs += p[i];
    return (cs == 0) ? SUCCESS : FAILURE;
}

static int __rf_return_check(rf_common_ret_t *r, int cmd)
{
    /* read state */
    if (SUCCESS != rf_read(r, (int)sizeof(*r))) {
        LOG_ERROR("read error");
        return FAILURE;
    }

    /* check rf return */
    if (r->header.head != HEAD_RF_TO_GATEWAY) {
        LOG_ERROR("head error: 0x%x, expect: 0x%x", r->header.head, HEAD_RF_TO_GATEWAY);
        return FAILURE;
    }
    if (r->header.cmd != cmd) {
        LOG_ERROR("cmd error: 0x%x, expect: 0x%x", r->header.cmd, cmd);
        return FAILURE;
    }
    if (r->header.len != sizeof(*r)) {
        LOG_ERROR("len error: %d, expect: %d", r->header.len, (int)sizeof(*r));
        return FAILURE;
    }
    if (SUCCESS != check_cs(r, sizeof(*r))) {
        LOG_ERROR("cs error");
        return FAILURE;
    }

    return SUCCESS;
}

static int __rf_send_cmd(void *body, int len, int cmd)
{
    char buffer[128 + DEFAULT_PACKET_SIZE];
    proto_head_t *h;
    char *b;
    rf_common_ret_t r;

    memset(buffer, 0, sizeof(buffer));
    memset(&r, 0, sizeof(r));

    h = (proto_head_t *)buffer;
    b = (char *)(buffer + sizeof(proto_head_t));

    /* init header */
    h->head = HEAD_GATEWAY_TO_RF;
    h->cmd = cmd;
    h->len = sizeof(proto_head_t) + len + sizeof(proto_tail_t);

    /* init body */
    memcpy(b, body, len);
    /* init tailer */
    calc_cs(h, h->len);

#ifdef RF_DEBUG
    LOG_INFO("Dump header:");
    dump_memory(h, sizeof(proto_head_t));
    LOG_INFO("Dump body:");
    dump_memory(b, len);
    LOG_INFO("Dump tailer:");
    dump_memory(b + len, sizeof(proto_tail_t));
#endif

    /* send cmd to rf */
    if (SUCCESS != rf_write(h, h->len)) {
        LOG_ERROR("write error");
        return FAILURE;
    }

    if (SUCCESS != __rf_return_check(&r, cmd)) {
        LOG_ERROR("rf return check error");
        return FAILURE;
    }

    if (r.state == RF_SUCCESS)
        return SUCCESS;
    LOG_ERROR("state : %d", r.state);
    return FAILURE;
}

int rf_config_sensor(config_sensor_body_t *b)
{
    return __rf_send_cmd(b, sizeof(*b), CMD_CONFIG_SENSOR);
}

int rf_config_basic_param(config_baisc_param_body_t *b)
{
    return __rf_send_cmd(b, sizeof(*b), CMD_CONFIG_BASIC_PARAM);
}

int rf_config_ext_param(config_ext_param_body_t *b)
{
    return __rf_send_cmd(b, sizeof(*b), CMD_CONFIG_EXT_PARAM);
}

int rf_upgrade(char *filename)
{
    rf_upgrade_body_t up;
    int totalsize;
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        LOG_ERROR("open %s failed", filename);
        return FAILURE;
    }

    memset(&up, 0, sizeof(up));
    totalsize = get_file_size(filename);
    up.totalpackets = totalsize / DEFAULT_PACKET_SIZE;
    if (totalsize % DEFAULT_PACKET_SIZE)
        up.totalpackets += 1;
    LOG_INFO("File: %s, Total size: %dbytes, Total packets: %d", filename, totalsize, up.totalpackets);

    for (int i = 0; i < (int)up.totalpackets; i++) {
        up.seqno = i + 1;
        up.packetsize = DEFAULT_PACKET_SIZE;
        if (up.seqno == up.totalpackets)
            /* last packet */
            up.packetsize = totalsize - (up.totalpackets - 1) * DEFAULT_PACKET_SIZE;
        if (read_exactly(fd, up.data, up.packetsize) != (int)up.packetsize)
            LOG_ERROR("read error");
        calc_cs(&up, sizeof(up));
#ifdef RF_DEBUG
        LOG_INFO("Seq: %d, Size: %d", up.seqno, up.packetsize);
#endif

        if (__rf_send_cmd(&up, sizeof(up), CMD_RF_UPGRADE) != SUCCESS) {
            LOG_ERROR("failed to send packe: %d, size: %d", up.seqno, up.packetsize);
            close(fd);
            return FAILURE;
        }
    }

    close(fd);
    return SUCCESS;
}

static int __read_rf_ack(void *head, void *body, void *tail)
{
    proto_head_t *h = (proto_head_t *)head;
    char *b = (char *)body;
    proto_tail_t *t = (proto_tail_t *)tail;
    int rc, size, i;
    unsigned char cs = 0;
    char *p;

    /* read the head */
    size = (int)sizeof(proto_head_t);
    rc = rf_read(h, size);
    if (rc != size) {
        LOG_ERROR("read head failed, expect %d, but %d", size, rc);
        return FAILURE;
    }

    /* sanity check */
    if (h->head != HEAD_RF_TO_GATEWAY) {
        LOG_ERROR("illegal head: 0x%x", h->head);
        return FAILURE;
    }

    /* calc the cs for head */
    p = (char *)h;
    for (i = 0; i < (int)sizeof(proto_head_t); i++)
        cs += p[i];

    /* read the body */
    size = h->len - sizeof(proto_head_t) - sizeof(proto_tail_t);
    rc = rf_read(b, size);
    if (rc != size) {
        LOG_ERROR("read body failed, expect %d, but %d", size, rc);
        return FAILURE;
    }

    /* calc the cs for head & body */
    p = (char *)b;
    for (i = 0; i < size; i++)
        cs += p[i];

    /* read the tail */
    size = (int)sizeof(proto_tail_t);
    rc = rf_read(t, size);
    if (rc != size) {
        LOG_ERROR("read tail failed, expect %d, but %d", size, rc);
        return FAILURE;
    }

    /* check the cs */
    if ((cs + t->cs) != 0) {
        LOG_ERROR("check cs failed, cs: 0x%x, t.cs: 0x%x", cs, t->cs);
        return FAILURE;
    }

    return SUCCESS;
}

static int __handle_rf_data_upload(void *body)
{
    rf_data_upload_body_t *b = (rf_data_upload_body_t *)body;
    LOG_INFO("SensorType: %d, ChannelNo: %d, SampleParamTime: %04d-%02d-%02d %02d:%02d:%02d, "
             "SampleParamVer: %d, SampleTime: %04d-%02d-%02d %02d:%02d:%02d, "
             "DataType: %d, Accoe: %f, TotalPackets: %d, Seq: %d, PacketSize: %d",
             b->sensortype, b->channelno,
             b->sampleparamtime.year, b->sampleparamtime.mon, b->sampleparamtime.day,
             b->sampleparamtime.hour, b->sampleparamtime.min, b->sampleparamtime.sec,
             b->sampleparamver,
             b->sampletime.year, b->sampletime.mon, b->sampletime.day,
             b->sampletime.hour, b->sampletime.min, b->sampletime.sec,
             b->datatype, b->accoe, b->totalpackets, b->seqno, b->packetsize);
    /* ack the rf */
    // TODO
    return SUCCESS;
}

int handle_rf_ack()
{
    proto_head_t h;
    proto_tail_t t;
    char body[MAX_BODY_SIZE];
    int rc = FAILURE;

    if (__read_rf_ack(&h, body, &t) != SUCCESS) {
        LOG_ERROR("read rf ack failed");
        return FAILURE;
    }

    switch (h.cmd) {
    case CMD_CONFIG_SENSOR:
        break;
    case CMD_CONFIG_BASIC_PARAM:
        break;
    case CMD_CONFIG_EXT_PARAM:
        break;
    case CMD_RF_UPGRADE:
        break;
    case CMD_SENSOR_UPGRADE:
        break;
    case CMD_DATA_UPLOAD:
        rc = __handle_rf_data_upload(body);
        break;
    default:
        LOG_ERROR("unknown cmd: 0x%x", h.cmd);
        rc = FAILURE;
        break;
    }

    return rc;
}
