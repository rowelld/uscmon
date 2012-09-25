#ifndef USBMON_H
#define USBMON_H

#include <exception>
#include <string>
#include <time.h>
#include <vector>
#include <stdint.h>
#include "usb.h"
#include "usbmon_defs.h"

class ErrorUsbmon: public std::exception
{
  virtual const char* what() const throw()  {return "Usbmon error";}
};

class ErrorUsbmonOpen: public ErrorUsbmon
{
  virtual const char* what() const throw()  {return "Device open error. Check for rights";}
};

class ErrorUsbmonIoctl: public ErrorUsbmon
{
  virtual const char* what() const throw()  {return "Device ioctl error";}
};

typedef enum {
	EP_TYPE_CONTROL_OUT,
	EP_TYPE_CONTROL_IN,
	EP_TYPE_ISOCRONOUS_OUT,
	EP_TYPE_ISOCRONOUS_IN,
	EP_TYPE_BULK_OUT,
	EP_TYPE_BULK_IN,
	EP_TYPE_INTERRUPT_OUT,
	EP_TYPE_INTERRUPT_IN,
	EP_TYPE_UNKNOWN
}EP_TYPE;

typedef enum {
	EVENT_SUBMISSION,
	EVENT_CALLBACK,
	EVENT_ERROR
}EVENT_TYPE;

typedef struct {
	time_t time;
	EVENT_TYPE event;
	int bus;
	int addr;
	EP_TYPE type;
	int ep;
	SETUP setup;
	unsigned int data_size;
	unsigned char* data;
}USB_REQUEST;

class Usbmon
{
private:
	int fileHandle;
	char* buf;
	mon_get_arg arg;

	std::string getNextWord(std::string& from, char delimiter);
	std::string read();
public:
	explicit Usbmon();
	virtual ~Usbmon();

	void open();
	void openBinary();
	void close();
	bool isActive() {return fileHandle != -1;}
	EVENT_TYPE readRequest(USB_REQUEST* req);
	EVENT_TYPE readRequestBinary(USB_REQUEST* req);
};

#endif // USBMON_H
