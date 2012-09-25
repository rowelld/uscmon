#ifndef USBCLASS_H
#define USBCLASS_H

#include "usbmon.h"
#include <vector>

typedef enum {
	LINE_CTS_ON,
	LINE_CTS_OFF,
	LINE_DSR_ON,
	LINE_DSR_OFF,
	LINE_RLSD_ON,
	LINE_RLSD_OFF,
	LINE_RING,
	LINE_BREAK,
	LINE_OVERRUN_ERROR,
	LINE_PARITY_ERROR,
	LINE_FRAME_ERROR
}LINE_EVENT;

typedef enum {
	FLOW_OFF,
	FLOW_RTS_CTS,
	FLOW_DTR_DSR,
	FLOW_XON_XOFF
}FLOW_EVENT;

class UsbClass
{
protected:
	int vid;
	int pid;

	void lineEvent(LINE_EVENT event);
	void flowEvent(FLOW_EVENT event);
	void dataFormatEvent(char parity, int dataBits, int stopBits);
	void baudRateEvent(int baudrate);
	void dataIn(unsigned char* data, unsigned int dataSize);
	void dataOut(unsigned char* data, unsigned int dataSize);
public:
	UsbClass(int vid, int pid);
	virtual ~UsbClass() {}

	virtual void classRequest(USB_REQUEST* req) = 0;
	virtual void vendorRequest(USB_REQUEST* req) = 0;
	virtual void epRequest(USB_REQUEST* req) = 0;
};

class UsbClassCreator
{
public:
	UsbClassCreator() {}
	virtual ~UsbClassCreator() {}

	virtual bool check(int vid, int pid) = 0;
	virtual UsbClass* create(int vid, int pid) = 0;
};

class UsbClassCreators
{
private:
	std::vector<UsbClassCreator*> creators;
public:
	UsbClassCreators() {}
	~UsbClassCreators();

	void registerCreator(UsbClassCreator* creator);
	UsbClass* create(int vid, int pid);
};

extern UsbClassCreators usbClassCreators;

#endif // USBCLASS_H
