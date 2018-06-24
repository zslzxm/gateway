#include "common.h"
#include <termios.h>

int uart_read(int fd, void *buf, int len)
{
    return read(fd, buf, len);
}

int uart_write(int fd, void *buf, int len)
{
    return write(fd, buf, len);
}

int uart_open(char *port, int speed, int bits, char parity, int stop)
{
    int fd = -1;
    fd = open(port, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        LOG_ERROR("open %s failed, fd = %d...", port, fd);
        goto err;
    }
#if 0
    // 判断串口的状态是否为阻塞状态
    ret = fcntl(fd, F_SETFL, 0);
    if(ret < 0) {
        LOG_ERROR("fcntl failed, ret = %d...\n", ret);
        goto err;
    }
#endif
    // 测试是否为终端设备
    if(isatty(fd) == 0) {
        LOG_ERROR("standard input is not a terminal device...");
        goto err;
    }

    struct termios newtio, oldio;
    bzero(&newtio, sizeof(newtio));

    if (tcgetattr(fd, &oldio) != 0) {
        LOG_ERROR("tcgetattr failed...");
        goto err;
    }

    // 修改控制模式，保证程序不会占用串口
    newtio.c_cflag |= CLOCAL;

    // 修改控制模式，使得能够从串口中读取输入数据
    newtio.c_cflag |= CREAD;

    // 设置数据位
    newtio.c_cflag &= ~CSIZE;  // 屏蔽其他标志位
    switch (bits) {
    case 7:
      newtio.c_cflag |= CS7;
      break;
    case 8:
      newtio.c_cflag |= CS8;
      break;
    }

    // 设置校验位
    switch (parity) {
    case 'O':
      newtio.c_cflag |= PARENB;
      newtio.c_cflag |= PARODD;
      newtio.c_iflag |= (INPCK | ISTRIP);
      break;
    case 'E':
      newtio.c_iflag |= (INPCK | ISTRIP);
      newtio.c_cflag |= PARENB;
      newtio.c_cflag &= ~PARODD;
      break;
    case 'N':
      newtio.c_cflag &= ~PARENB;
      break;
    }

    // 设置串口输入波特率和输出波特率
    switch (speed) {
    case 2400:
      cfsetispeed(&newtio, B2400);
      cfsetospeed(&newtio, B2400);
      break;
    case 4800:
      cfsetispeed(&newtio, B4800);
      cfsetospeed(&newtio, B4800);
      break;
    case 9600:
      cfsetispeed(&newtio, B9600);
      cfsetospeed(&newtio, B9600);
      break;
    case 115200:
      cfsetispeed(&newtio, B115200);
      cfsetospeed(&newtio, B115200);
      break;
    default:
      cfsetispeed(&newtio, B115200);
      cfsetospeed(&newtio, B115200);
      break;
    }

    // 设置停止位
    if(stop == 1)
        newtio.c_cflag &= ~CSTOPB;
    else if (stop == 2)
        newtio.c_cflag |= CSTOPB;

    newtio.c_iflag = 0;
    newtio.c_oflag = 0;

    //设置等待时间和最小接收字符
    newtio.c_cc[VTIME] = 1;  //单位: 百毫秒
    newtio.c_cc[VMIN] = 0;
    tcflush(fd, TCIOFLUSH);

    if((tcsetattr(fd, TCSANOW, &newtio)) != 0) {
      LOG_ERROR("tcsetattr failed...");
      goto err;
    }

    LOG_INFO("open %s success, fd = %d...", port, fd);
    return fd;
err:
    if (fd >= 0)
        close(fd);
    return -1;
}

int uart_close(int fd)
{
    return close(fd);
}
