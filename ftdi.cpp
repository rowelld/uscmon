#include "ftdi.h"
#include "ftdi_sio.h"
#include <stdio.h>

//from ftdi_sio.c
#define FTDI_STATUS_B0_MASK	(FTDI_RS0_CTS | FTDI_RS0_DSR | FTDI_RS0_RI | FTDI_RS0_RLSD)
#define FTDI_STATUS_B1_MASK	(FTDI_RS_BI | FTDI_RS_OE | FTDI_RS_PE | FTDI_RS_FE)

FTDI::FTDI(int vid, int pid) :
	UsbClass(vid, pid)
{
}

FTDI::~FTDI()
{
}


bool FTDI::modemControl(USB_REQUEST *req)
{
	bool handled = false;
	switch (req->setup.wValue)
	{
	case FTDI_SIO_SET_DTR_HIGH:
		printf("DTR ON\n");
		handled = true;
		break;
	case FTDI_SIO_SET_DTR_LOW:
		printf("DTR OFF\n");
		handled = true;
		break;
	case FTDI_SIO_SET_RTS_HIGH:
		printf("RTS ON\n");
		handled = true;
		break;
	case FTDI_SIO_SET_RTS_LOW:
		printf("RTS OFF\n");
		handled = true;
		break;
	}
	return handled;
}

bool FTDI::flowControl(USB_REQUEST *req)
{
	bool handled = false;
	switch (req->setup.wValue)
	{
	case FTDI_SIO_DISABLE_FLOW_CTRL:
		flowEvent(FLOW_OFF);
		handled = true;
		break;
	case FTDI_SIO_RTS_CTS_HS:
		flowEvent(FLOW_RTS_CTS);
		handled = true;
		break;
	case FTDI_SIO_DTR_DSR_HS:
		flowEvent(FLOW_DTR_DSR);
		handled = true;
		break;
	case FTDI_SIO_XON_XOFF_HS:
		flowEvent(FLOW_XON_XOFF);
		handled = true;
		break;
	}
	return handled;
}

bool FTDI::setBaudRate(USB_REQUEST *req)
{
	bool handled = false;
	switch (req->setup.wValue >> 8)
	{
	case ftdi_sio_b300:
		baudRateEvent(300);
		handled = true;
		break;
	case ftdi_sio_b600:
		baudRateEvent(600);
		handled = true;
		break;
	case ftdi_sio_b1200:
		baudRateEvent(1200);
		handled = true;
		break;
	case ftdi_sio_b2400:
		baudRateEvent(2400);
		handled = true;
		break;
	case ftdi_sio_b4800:
		baudRateEvent(4800);
		handled = true;
		break;
	case ftdi_sio_b9600:
		baudRateEvent(9600);
		handled = true;
		break;
	case ftdi_sio_b19200:
		baudRateEvent(19200);
		handled = true;
		break;
	case ftdi_sio_b38400:
		baudRateEvent(38400);
		handled = true;
		break;
	case ftdi_sio_b57600:
		baudRateEvent(57600);
		handled = true;
		break;
	case ftdi_sio_b115200:
		baudRateEvent(115200);
		handled = true;
		break;
	default:
		printf("Unknown baud value %x\n", req->setup.wValue);
	}

	return handled;
}

bool FTDI::dataControl(USB_REQUEST *req)
{
	char parity = 'N';
	switch (req->setup.wValue & (7 << 8))
	{
	case FTDI_SIO_SET_DATA_PARITY_ODD:
		parity = 'O';
		break;
	case FTDI_SIO_SET_DATA_PARITY_EVEN:
		parity = 'E';
		break;
	case FTDI_SIO_SET_DATA_PARITY_MARK:
		parity = 'M';
		break;
	case FTDI_SIO_SET_DATA_PARITY_SPACE:
		parity = ' ';
		break;
	}
	int stopBits = 1;
	switch (req->setup.wValue & (7 << 11))
	{
	case FTDI_SIO_SET_DATA_STOP_BITS_15:
		stopBits = 15;
		break;
	case FTDI_SIO_SET_DATA_STOP_BITS_2:
		stopBits = 2;
		break;
	}
	dataFormatEvent(parity, 8, stopBits);
	if (req->setup.wValue & FTDI_SIO_SET_BREAK)
		lineEvent(LINE_BREAK);
	return true;
}

void FTDI::checkStatus0(unsigned char status0)
{
	if (FTDI::status0 != (status0 & FTDI_STATUS_B0_MASK))
	{
		if ((FTDI::status0 ^ status0) & FTDI_RS0_CTS)
			lineEvent(status0 & FTDI_RS0_CTS ? LINE_CTS_ON : LINE_CTS_OFF);
		if ((FTDI::status0 ^ status0) & FTDI_RS0_DSR)
			lineEvent(status0 & FTDI_RS0_DSR ? LINE_DSR_ON : LINE_DSR_OFF);
		if ((FTDI::status0 & FTDI_RS0_RI) == 0 && (status0 & FTDI_RS0_RI))
			lineEvent(LINE_RING);
		if ((FTDI::status0 ^ status0) & FTDI_RS0_RLSD)
			lineEvent(status0 & FTDI_RS0_RLSD ? LINE_RLSD_ON : LINE_RLSD_OFF);
		FTDI::status0 = status0 & FTDI_STATUS_B0_MASK;
	}
}


void FTDI::checkStatus1(unsigned char status1)
{
	if (FTDI::status1 != (status1 & FTDI_STATUS_B1_MASK))
	{
		if ((FTDI::status1 & FTDI_RS_BI) == 0 && (status1 & FTDI_RS_BI))
			lineEvent(LINE_BREAK);
		if ((FTDI::status1 & FTDI_RS_OE) == 0 && (status1 & FTDI_RS_OE))
			lineEvent(LINE_OVERRUN_ERROR);
		if ((FTDI::status1 & FTDI_RS_PE) == 0 && (status1 & FTDI_RS_PE))
			lineEvent(LINE_PARITY_ERROR);
		if ((FTDI::status1 & FTDI_RS_FE) == 0 && (status1 & FTDI_RS_FE))
			lineEvent(LINE_FRAME_ERROR);
		FTDI::status1 = status1 & FTDI_STATUS_B1_MASK;
	}
}

void FTDI::classRequest(USB_REQUEST *req)
{
	printf("%d:%d class SETUP: %02x,%02x,%04x,%04x,%04x\n", req->bus, req->addr, req->setup.bmRequestType, req->setup.bRequest, req->setup.wValue, req->setup.wIndex, req->setup.wLength);
}

void FTDI::vendorRequest(USB_REQUEST *req)
{
	bool handled = false;
	switch (req->setup.bRequest)
	{
	case FTDI_SIO_MODEM_CTRL:
		handled = modemControl(req);
		break;
	case FTDI_SIO_SET_FLOW_CTRL:
		handled = flowControl(req);
		break;
	case FTDI_SIO_SET_BAUD_RATE:
		handled = setBaudRate(req);
		break;
	case FTDI_SIO_SET_DATA:
		handled = dataControl(req);
		break;
	case FTDI_SIO_RESET:
	case FTDI_SIO_GET_MODEM_STATUS:
	case FTDI_SIO_SET_EVENT_CHAR:
	case FTDI_SIO_SET_ERROR_CHAR:
	case FTDI_SIO_SET_LATENCY_TIMER:
	case FTDI_SIO_GET_LATENCY_TIMER:
	//win xp asks this value on init. Have no idea wtf.
	case 0x90:
		handled = true;
		break;
	}
	if (!handled)
		printf("%d:%d vendor SETUP: %02x,%02x,%04x,%04x,%04x\n", req->bus, req->addr, req->setup.bmRequestType, req->setup.bRequest, req->setup.wValue, req->setup.wIndex, req->setup.wLength);
}

void FTDI::epRequest(USB_REQUEST *req)
{
	if (req->type == EP_TYPE_INTERRUPT_IN || req->type == EP_TYPE_BULK_IN)
	{
		if (req->data_size >= 2)
		{
			checkStatus0(req->data[0]);
			checkStatus1(req->data[1]);
			if (req->data_size > 2)
				dataIn(req->data + 2, req->data_size - 2);
		}
	}
	else
	{
		dataOut(req->data, req->data_size);
	}
}
