#ifndef USBMON_DEFS_H
#define USBMON_DEFS_H

#include <sys/ioctl.h>


#define SETUP_LEN  8
#define USBMON_MAX_SIZE			65536

#pragma pack(push, 1)

struct usbmon_packet {
	uint64_t id;			/*  0: URB ID - from submission to callback */
	unsigned char type;	/*  8: Same as text; extensible. */
	unsigned char xfer_type; /*    ISO (0), Intr, Control, Bulk (3) */
	unsigned char epnum;	/*     Endpoint number and transfer direction */
	unsigned char devnum;	/*     Device address */
	uint16_t busnum;		/* 12: Bus number */
	char flag_setup;	/* 14: Same as text */
	char flag_data;		/* 15: Same as text; Binary zero is OK. */
	int64_t ts_sec;		/* 16: gettimeofday */
	int32_t ts_usec;		/* 24: gettimeofday */
	int status;		/* 28: */
	unsigned int length;	/* 32: Length of data (submitted or actual) */
	unsigned int len_cap;	/* 36: Delivered length */
	union {			/* 40: */
		unsigned char setup[SETUP_LEN];	/* Only for Control S-type */
		struct iso_rec {		/* Only for ISO */
			int error_count;
			int numdesc;
		} iso;
	} s;
	int interval;		/* 48: Only for Interrupt and ISO */
	int start_frame;	/* 52: For ISO */
	unsigned int xfer_flags; /* 56: copy of URB's transfer_flags */
	unsigned int ndesc;	/* 60: Actual number of ISO descriptors */
};				/* 64 total length */

struct mon_get_arg {
	struct usbmon_packet *hdr;
	unsigned char *data;
	size_t alloc;		/* Length of data (can be zero) */
};

#define MON_IOC_MAGIC 0x92

#define MON_IOCX_GETX							_IOW(MON_IOC_MAGIC, 10, struct mon_get_arg)

#pragma pack(pop)


#endif // USBMON_DEFS_H
