#ifndef USBDISPATCHER_H
#define USBDISPATCHER_H

#include "usbmon.h"
#include "usbdevice.h"

class UsbDispatcher
{
private:
	Usbmon usbmon;
	std::vector<USB_REQUEST*>incompleteRequests;
	std::vector<UsbDevice*>devices;
public:
	explicit UsbDispatcher();
	virtual ~UsbDispatcher();

	void start();
};

#endif // USBDISPATCHER_H
