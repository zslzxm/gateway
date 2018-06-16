
int spi_write(int fd, char *buf, int len)
{
    int rc;
    rc = write(fd, buf, len);
    if (rc != len)
        LOG_WARNING("expect %d, but %d", len, rc);
    return rc;
}

int spi_read(int fd, char *buf, int len)
{
    int rc;
    rc = read(fd, buf, len);
    if (rc != len)
        LOG_WARNING("expect %d, but %d", len, rc);
    return rc;
}

int spi_open(const char *device, unsigned char mode, unsigned char bits, unsigned int speed)
{
    int fd = -1;
    int rc = 0;

    if (!device) {
        LOG_ERROR("must specify a spi device");
        goto err;
    }

    fd = open(device, O_RDWR);
    if (fd < 0) {
        LOG_ERROR("can't open spi device: %s", device);
        goto err;
    }

    LOG_INFO("Open device: %s with mode: %d, bits: %d, speed: %dHZ", device, mode, bits, speed);

    /* spi mode */
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
        LOG_ERROR("can't set spi mode: %d", mode);
        rc = -1;
        goto err;
    }

    if (ioctl(fd, SPI_IOC_RD_MODE, &mode) < 0) {
        LOG_ERROR("can't get spi mode: %d", mode);
        rc = -1;
        goto err;
    }

    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
        LOG_ERROR("can't set bits: %d", bits);
        rc = -1;
        goto err;
    }

    if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0) {
        LOG_ERROR("can't get bits: %d", bits);
        rc = -1;
        goto err;
    }

    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        LOG_ERROR("can't set speed: %d", speed);
        rc = -1;
        goto err;
    }

    if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0) {
        LOG_ERROR("can't get speed: %d", speed);
        rc = -1;
        goto err;
    }

    LOG_INFO("Successfully open the spi device");
err:
    if (rc < 0 && fd > 0) {
        close(fd);
        fd = -1;
        LOG_INFO("Failed to open the spi device");
    }

    return fd;
}

int spi_close(int fd)
{
    close(fd);
    return 0;
}
