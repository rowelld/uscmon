TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp \
    usbmon.cpp \
    usbdevice.cpp \
    usbdispatcher.cpp \
    usbclass.cpp \
    ftdi.cpp

HEADERS += \
    usbmon.h \
    usbdevice.h \
    usbdispatcher.h \
    usb.h \
    usbmon_defs.h \
    usbclass.h \
    ftdi.h \
    ftdi_sio.h

