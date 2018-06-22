#ifndef __RF_OP_H__
#define __RF_OP_H__

#ifdef  __cplusplus
extern "C" {
#endif

enum {
    RF_INTERFACE_SPI = 0,
    RF_INTERFACE_UART,
};

int rf_open(int interface);
int rf_close();
int rf_read(void *buf, int len);
int rf_write(void *buf, int len);

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif
