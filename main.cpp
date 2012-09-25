#include <iostream>
#include "usbmon.h"
#include <string>
#include <stdio.h>
#include "usbdispatcher.h"
#include "usbclass.h"
#include "ftdi.h"

UsbClassCreators usbClassCreators;

int main()
{
	printf("USB serial converter monitor\n");

	usbClassCreators.registerCreator(new FTDICreator);
	UsbDispatcher dispatcher;
	try
	{
		dispatcher.start();
	}
	catch (std::exception& e)
	{
		printf("Error: %s\n", e.what());
	}
	return 0;
}

