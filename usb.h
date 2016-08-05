#ifndef USB_H
#define USB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

int init_libusb(libusb_context **ctx);
void close_device(libusb_device_handle *beeprinter);
void close_libusb(libusb_context **ctx);
int get_device_list(libusb_context **ctx, libusb_device ***devs);
libusb_device_handle* get_first_printer(libusb_context **ctx);
int send_message(libusb_device_handle *handle, unsigned char *message, 
        int message_size, int timeout);
int receive_message(libusb_device_handle *handle, 
        unsigned char *message_buffer, int expected_size, int timeout);
#endif /* USB_H */
