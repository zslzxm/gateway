#include "rf-op.h"

static int gRfInterface = -1;
static int gRfFd = -1;

static void __rf_power(int on)
{
    
}

int rf_open(int interface)
{
    gRfInterface = interface;
    __rf_power(0);
    __rf_power(1);

    if (interface == RF_INTERFACE_SPI) {
        gRfFd = spi_open("/dev/spidev1.0", 0, 8, 500 * 1000);
    } else if (interface == RF_INTERFACE_UART) {
        
    }

    return gRfFd;
}

int rf_close()
{
    close(gRfFd);
    __rf_power(0);
    return 0;
}

int rf_read(char *buf, int len)
{
    if (gRfInterface == RF_INTERFACE_SPI)
        return spi_read(gRfFd, buf, len);
}

int rf_write(char *buf, int len)
{
    if (gRfInterface == RF_INTERFACE_SPI)
        return spi_write(gRfFd, buf, len);
}
