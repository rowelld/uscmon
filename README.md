USB serial monitor
==================

uscmon is serial port sniffer for USB serial converters, like FTDI. For now, only FTDI is supported.

Following events are supported:

- data transfers
- baud rate changing
- parity, data, stop bits changing
- flow control changing
- DTR/DSR, RTS/CTS, RI events
- break, overrun/parity/frame error events

Usage
=====

sudo ./uscmon

Make sure, debug_fs is enabled and usbmon module is loaded. Read usbmon info for more information.
