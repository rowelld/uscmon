#ifndef FTDI_H
#define FTDI_H

#include "usbclass.h"

class FTDI : public UsbClass
{
private:
	unsigned char status0, status1;
	bool modemControl(USB_REQUEST* req);
	bool flowControl(USB_REQUEST* req);
	bool setBaudRate(USB_REQUEST* req);
	bool dataControl(USB_REQUEST* req);

	void checkStatus0(unsigned char status0);
	void checkStatus1(unsigned char status1);
public:
	FTDI(int vid, int pid);
	virtual ~FTDI();

	virtual void classRequest(USB_REQUEST* req);
	virtual void vendorRequest(USB_REQUEST* req);
	virtual void epRequest(USB_REQUEST* req);
};

class FTDICreator : public UsbClassCreator
{
public:
	FTDICreator() : UsbClassCreator() {}
	virtual ~FTDICreator() {}

	virtual bool check(int vid, int pid) {(void)pid; return vid == 0x0403;}
	virtual UsbClass* create(int vid, int pid) {return new FTDI(vid, pid);}
};

#endif // FTDI_H
