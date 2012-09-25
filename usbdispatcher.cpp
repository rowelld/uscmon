#include "usbdispatcher.h"
#include <stdio.h>

UsbDispatcher::UsbDispatcher()
{
}

UsbDispatcher::~UsbDispatcher()
{
	usbmon.close();
	for (std::vector<UsbDevice*>::iterator it = devices.begin(); it < devices.end(); ++it)
		delete *it;
	for (std::vector<USB_REQUEST*>::iterator it = incompleteRequests.begin(); it < incompleteRequests.end(); ++it)
	{
		delete (*it)->data;
		delete *it;
	}
}

void UsbDispatcher::start()
{
	usbmon.openBinary();
	USB_REQUEST* req;
	for (;;)
	{
		req = new USB_REQUEST;
		EVENT_TYPE et = usbmon.readRequestBinary(req);
		bool completed = false;
		for (std::vector<USB_REQUEST*>::iterator it = incompleteRequests.begin(); it < incompleteRequests.end(); ++it)
		{
			if ((*it)->bus == req->bus && (*it)->addr == req->addr && (*it)->ep == req->ep &&
				 (*it)->type == req->type)
			{
				if (req->data_size)
				{
					(*it)->data = req->data;
					(*it)->data_size = req->data_size;
				}
				delete req;
				req = *it;
				incompleteRequests.erase(it);
				completed = true;
				break;
			}
		}
		if (et != EVENT_ERROR)
		{
			if (completed)
			{
				bool handled = false;
				for (std::vector<UsbDevice*>::iterator it = devices.begin(); it < devices.end(); ++it)
				{
					if ((*it)->getBus() == req->bus && (*it)->getAddr() == req->addr)
					{
						handled = true;
						switch((*it)->request(req))
						{
						case USB_NO_HANDLER:
						case USB_SUSPENDED:
							delete *it;
							devices.erase(it);
							break;
						default:
							break;
						}

						break;
					}
				}
				if (!handled && req->addr == 0)
				{
					UsbDevice* dev = new UsbDevice(req->bus);
					dev->request(req);
					devices.push_back(dev);
				}
				delete[] req->data;
				delete req;
			}
			else
				incompleteRequests.push_back(req);
		}
		else
		{
			delete[] req->data;
			delete req;
		}

	}
	usbmon.close();
}
