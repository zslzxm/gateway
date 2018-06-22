TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CFLAGS += -std=c99

SOURCES += main.c \
    drivers/spi/spi.c \
    drivers/uart/uart.c \
    protocols/rf-gateway/rf-op.c \
    protocols/rf-gateway/rf-protocol.c \
    helper-function.c

HEADERS += \
    common.h \
    drivers/spi/spi.h \
    drivers/uart/uart.h \
    protocols/rf-gateway/rf-op.h \
    protocols/rf-gateway/rf-protocol.h \
    helper-function.h
