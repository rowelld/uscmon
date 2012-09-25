#ifndef USBDEVICE_H
#define USBDEVICE_H

#include "usbmon.h"
#include <vector>
#include <string>
#include "usb.h"
#include "usbclass.h"

typedef enum {
	USB_OK,
	USB_CONFIGURED,
	USB_NO_HANDLER,
	USB_SUSPENDED
}USB_RES;

class UsbDevice
{
private:
	int vid, pid, bus, addr;
	int productIndex, manufacturerIndex, serialIndex;
	std::string product;
	std::string manufacturer;
	std::string serial;
	bool configured;
	UsbClass* usbClass;

	std::string toNarrow(uint16_t* wide, int size);
	USB_RES deviceConfigured();
	void getDescriptor(USB_REQUEST *req);
	USB_RES deviceRequest(USB_REQUEST *req);
public:
	explicit UsbDevice(int bus);

	USB_RES request(USB_REQUEST* req);
	int getVid() {return vid;}
	int getPid() {return pid;}
	int getBus() {return bus;}
	int getAddr() {return addr;}
	const std::string& getProduct() {return product;}
	const std::string& getManufacturer() {return manufacturer;}
	const std::string& getSerial() {return serial;}
};

#endif // USBDEVICE_H
