#ifndef __UART_H__
#define __UART_H__

#ifdef  __cplusplus
extern "C" {
#endif

int uart_read(int fd, void *buf, int len);
int uart_write(int fd, void *buf, int len);
int uart_open(char *port, int speed, int bits, char parity, int stop);
int uart_close(int fd);

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif // __UART_H__
