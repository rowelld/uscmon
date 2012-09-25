#include "usbdevice.h"
#include <stdio.h>

UsbDevice::UsbDevice(int bus) :
	vid(0),
	pid(0),
	addr(0),
	productIndex(-1),
	manufacturerIndex(-1),
	serialIndex(-1),
	configured(false)
{
	UsbDevice::bus = bus;
}

std::string UsbDevice::toNarrow(uint16_t* wide, int size)
{
	std::string res;
	for(int i = 0; i < size && wide[i]; ++i)
		res.push_back(static_cast<char>(wide[i] & 0xff));
	return res;
}

USB_RES UsbDevice::deviceConfigured()
{
	USB_RES res = USB_OK;
	usbClass = usbClassCreators.create(vid, pid);
	if (usbClass)
	{
		printf("new device found: %d:%d %04X:%04X", bus, addr, vid, pid);
		if (product.size())
			printf(" %s", product.c_str());
		if (manufacturer.size())
			printf(", %s", manufacturer.c_str());
		if (serial.size())
			printf(", serial: %s", serial.c_str());
		printf("\n");
		res = USB_CONFIGURED;
	}
	else
		res = USB_NO_HANDLER;
	return res;
}

void UsbDevice::getDescriptor(USB_REQUEST *req)
{
	USB_DEVICE_DESCRIPTOR_TYPE* deviceDescriptor;
	int index = req->setup.wValue & 0xff;
	switch (req->setup.wValue >> 8)
	{
	case USB_DEVICE_DESCRIPTOR_INDEX:
		if (req->data_size >= sizeof(USB_DEVICE_DESCRIPTOR_TYPE))
		{
			deviceDescriptor = reinterpret_cast<USB_DEVICE_DESCRIPTOR_TYPE*>(req->data);
			pid = deviceDescriptor->idProduct;
			vid = deviceDescriptor->idVendor;
			productIndex = deviceDescriptor->iProduct;
			manufacturerIndex = deviceDescriptor->iManufacturer;
			serialIndex = deviceDescriptor->iSerialNumber;
		}
		break;
	case USB_STRING_DESCRIPTOR_INDEX:
		if (index && req->data_size > 2)
		{
			std::string str(toNarrow(reinterpret_cast<uint16_t*>(req->data + 2), (req->data_size - 2) / 2));
			if (index == productIndex)
				product = toNarrow(reinterpret_cast<uint16_t*>(req->data + 2), (req->data_size - 2) / 2);
			if (index == manufacturerIndex)
				manufacturer = toNarrow(reinterpret_cast<uint16_t*>(req->data + 2), (req->data_size - 2) / 2);
			if (index == serialIndex)
				serial = toNarrow(reinterpret_cast<uint16_t*>(req->data + 2), (req->data_size - 2) / 2);
		}
		break;
	}
}

USB_RES UsbDevice::deviceRequest(USB_REQUEST *req)
{
	USB_RES res = USB_OK;
	switch (req->setup.bRequest)
	{
	case USB_REQUEST_SET_ADDRESS:
		addr = req->setup.wValue;
		break;
	case USB_REQUEST_GET_DESCRIPTOR:
		getDescriptor(req);
		break;
	case USB_REQUEST_SET_CONFIGURATION:
		configured = true;
		res = deviceConfigured();
		break;
	default:
		break;
	}
	return res;
}

USB_RES UsbDevice::request(USB_REQUEST *req)
{
	USB_RES res = USB_OK;
	if (req->ep == 0)
	{
		switch (req->setup.bmRequestType & BM_REQUEST_TYPE)
		{
		case BM_REQUEST_TYPE_STANDART:
			res = deviceRequest(req);
			break;
		case BM_REQUEST_TYPE_CLASS:
			if (configured)
				usbClass->classRequest(req);
			break;
		case BM_REQUEST_TYPE_VENDOR:
			if (configured)
				usbClass->vendorRequest(req);
			break;
		}
	}
	else if (configured)
		usbClass->epRequest(req);
	return res;
}
