#include "rf-protocol.h"
#include "common.h"

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

static int __rf_config(void *body, int len, int cmd)
{
    char buffer[1024];
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
    return __rf_config(b, sizeof(*b), CMD_CONFIG_SENSOR);
}

int rf_config_basic_param(config_baisc_param_body_t *b)
{
    return __rf_config(b, sizeof(*b), CMD_CONFIG_BASIC_PARAM);
}

int rf_config_ext_param(config_ext_param_body_t *b)
{
    return __rf_config(b, sizeof(*b), CMD_CONFIG_EXT_PARAM);
}

int rf_upgrade(char *filename)
{
    rf_upgrade_t up;
    int totalsize;
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        LOG_ERROR("open %s failed", filename);
        return FAILURE;
    }

    memset(&up, 0, sizeof(up));
    up.header.head = HEAD_GATEWAY_TO_RF;
    up.header.cmd = CMD_RF_UPGRADE;
    up.header.len = sizeof(up);

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
        read(fd, up.data, up.packetsize);
        calc_cs(&up, sizeof(up));
    }

    close(fd);
    return SUCCESS;
}
