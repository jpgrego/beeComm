#ifndef USB_H
#define USB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

int init_libusb(libusb_context **ctx);
int get_device_list(libusb_context **ctx, libusb_device ***devs);
libusb_device_handle* get_first_printer(libusb_device **device_list, 
        size_t device_list_size);

#endif /* USB_H */
