#ifndef __RF_PROTOCOL_H__
#define __RF_PROTOCOL_H__

#ifdef  __cplusplus
extern "C" {
#endif
#include "rf-op.h"

/* Protocol: HEAD(1B)+CMD(1B)+LEN(2B)+DATA+CS(1B) */

/* define the head */
#define HEAD_RF_TO_GATEWAY  0x56
#define HEAD_GATEWAY_TO_RF  0x61

/* define the cmd */
#define CMD_CONFIG_SENSOR       0x0F
#define CMD_CONFIG_BASIC_PARAM  0x00
#define CMD_CONFIG_EXT_PARAM    0x04
#define CMD_RF_UPGRADE          0x10
#define CMD_SENSOR_UPGRADE      0x11
#define CMD_DATA_UPLOAD         0x21

/* define the structure */
typedef struct {
    unsigned char   head;
    unsigned char   cmd;
    unsigned short  len;
}__attribute__((packed)) proto_head_t;

typedef struct {
    unsigned char   tail;
}__attribute__((packed)) proto_tail_t;

typedef struct {
    unsigned short  year;
    unsigned char   mon;
    unsigned char   day;
    unsigned char   hour;
    unsigned char   min;
    unsigned char   sec;
    unsigned short  ms;
}__attribute__((packed)) proto_time_t;

typedef struct {
    proto_head_t header;
    unsigned short  paramhead;
    unsigned short  panid;
    unsigned short  sensoraddr;
    unsigned short  rfaddr;
    unsigned char   freq;
    unsigned char   sendpower;
    unsigned int    sn;
    unsigned short  hwver;
    float           accoe;
    unsigned char   rsvd[11];
    unsigned char   wakeuptime;
    unsigned short  reconnectinterval;
    unsigned short  paramtail;
    proto_tail_t tailer;
}__attribute__((packed))  config_sensor_t;

typedef struct {
    unsigned short  analysisfreq;
    unsigned short  samplelen;
    unsigned long   id;
}__attribute__((packed))  meas_def_t;

typedef struct {
    proto_head_t header;
    proto_time_t    servertime;
    proto_time_t    sampleparamtime;
    unsigned char   sampleparamver;
    unsigned char   rfstate;
    unsigned short  sleeptime;
    unsigned char   workmode;
    unsigned char   upgradeflag;
    unsigned char   stopflag;
    unsigned char   paramupdateflag;
    unsigned char   devicetype;
    unsigned char   vibstartchannelno;
    unsigned char   vibstopchannelno;
    unsigned char   rpmstartchannelno;
    unsigned char   rpmstopchannelno;
    unsigned char   tempstartchannelno;
    unsigned char   tempstopchannelno;
    unsigned short  measdefsampleinterval;
    unsigned short  timedomainsampleinterval;
    unsigned short  longwavesampleinterval;
    unsigned short  rpmsampleinterval;
    unsigned short  rpmuploadinterval;
    unsigned short  tempampleinterval;
    unsigned short  tempuploadinterval;
    unsigned short  longwaveanalysisfreq;
    unsigned short  longwavesamplelen;
    meas_def_t      measdef[3];
    unsigned long   pointid;
    unsigned char   unit;
    unsigned short  attachlen;
    unsigned short  rpmtriggersamplefreq;
    unsigned short  rpmtriggersamplelen;
    unsigned long   rpmrange1;
    unsigned long   rpmrange2;
    unsigned long   rpmrange3;
    float           totalvalue4levelthreshold;
    float           totalvalue2levelchangerate;
    float           peakvalue4levelthreshold;
    float           peakvalue2levelchangerate;
    unsigned char   alarmsamplecount;
    unsigned short  sensoruplodinterval;
    proto_tail_t tailer;
}__attribute__((packed))  config_baisc_param_t;

typedef struct {
    proto_head_t header;
    unsigned char   indexthresholdresetinterval;
    unsigned char   indexthresholdupdatestartgroups;
    unsigned char   highfreqindexsamplelen;
    unsigned char   lowfreqindexsamplelen;
    unsigned short  highfreqrange1left;
    unsigned short  highfreqrange1right;
    unsigned short  highfreqrange2left;
    unsigned short  highfreqrange2right;
    unsigned short  highfreqrange3left;
    unsigned short  highfreqrange3right;
    unsigned short  highfreqrange4left;
    unsigned short  highfreqrange4right;
    unsigned short  lowfreqrange1left;
    unsigned short  lowfreqrange1right;
    unsigned short  lowfreqrange2left;
    unsigned short  lowfreqrange2right;
    unsigned short  lowfreqrange3left;
    unsigned short  lowfreqrange3right;
    unsigned short  lowfreqrange4left;
    unsigned short  lowfreqrange4right;
    float           highfreqtotalvaluealarmthreshold;
    // TODO
    proto_tail_t tailer;
}__attribute__((packed))  config_ext_param_t;

typedef struct {
    proto_head_t header;
    unsigned int    totalpackets;
    unsigned int    seqno;
    unsigned int    packetsize;
    unsigned char   data[0];
    //proto_tail_t tailer;
}__attribute__((packed))  rf_upgrade_t;

typedef struct {
    proto_head_t header;
    unsigned int    totalpackets;
    unsigned int    seqno;
    unsigned int    packetsize;
    unsigned char   data[0];
    //proto_tail_t tailer;
}__attribute__((packed))  sensor_upgrade_t;

typedef struct {
    proto_head_t header;
    unsigned char   sensortype;
    unsigned char   channelno;
    proto_time_t    sampleparamtime;
    unsigned char   sampleparamver;
    proto_time_t    sampletime;
    unsigned char   datatype;
    float           accoe;
    unsigned int    totalpackets;
    unsigned int    seqno;
    unsigned int    packetsize;
    unsigned char   data[0];
    //proto_tail_t tailer;
}__attribute__((packed))  data_upload_t;

typedef struct {
    proto_head_t header;
    unsigned char state;
    proto_tail_t tailer;
}__attribute__((packed))  gateway_ret_t;

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif
