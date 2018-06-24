#include "rf-op.h"
#include "common.h"
#include "drivers/spi/spi.h"
#include "drivers/uart/uart.h"

static int gRfInterface = -1;
static int gRfFd = -1;

static void __rf_power(int on)
{
    LOG_INFO("rf power %s", on ? "on" : "off");
}

int rf_open(int interface)
{
    gRfInterface = interface;
    __rf_power(0);
    __rf_power(1);

    if (interface == RF_INTERFACE_SPI) {
        gRfFd = spi_open("/dev/spidev1.0", 0, 8, 500 * 1000);
    } else if (interface == RF_INTERFACE_UART) {
        gRfFd = uart_open("/dev/ttyS5", 115200, 8, 'N', 1);
    }

    return gRfFd;
}

int rf_close()
{
    if (gRfInterface == RF_INTERFACE_SPI)
        spi_close(gRfFd);
    else if (gRfInterface == RF_INTERFACE_UART)
        uart_close(gRfFd);
    __rf_power(0);
    return 0;
}

int rf_read(void *buf, int len)
{
    if (gRfInterface == RF_INTERFACE_SPI)
        return spi_read(gRfFd, (char *)buf, len);
    else if (gRfInterface == RF_INTERFACE_UART)
        return uart_read(gRfFd, buf, len);
    return 0;
}

int rf_write(void *buf, int len)
{
    if (gRfInterface == RF_INTERFACE_SPI)
        return spi_write(gRfFd, (char *)buf, len);
    else if (gRfInterface == RF_INTERFACE_UART)
        return uart_write(gRfFd, buf, len);
    return 0;
}
