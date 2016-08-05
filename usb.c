#include "usb.h"
#include "debug.h"

#define BEETHEFIRST_ENDPOINT_OUT 5
#define BEETHEFIRST_ENDPOINT_IN  2

int init_libusb(libusb_context **ctx) {
    int retval;

    retval = libusb_init(ctx);

    if(retval < 0) {
        DEBUG_PRINT(("(Initialing LIBUSB) %s (%d): %s\n", 
                    libusb_error_name(retval), retval, 
                    libusb_strerror(retval)));
    } else {
        libusb_set_debug(*ctx, 3);
    }

    return retval;
}

void close_libusb(libusb_context **ctx) {
    if(ctx != NULL) {
        libusb_exit(*ctx);
    }
}

void close_device(libusb_device_handle *beeprinter) {
    if(beeprinter != NULL) {
        libusb_release_interface(beeprinter, 0);
        libusb_close(beeprinter);
    }
}

int get_device_list(libusb_context **ctx, libusb_device ***devs) {
    size_t cnt;
    
    cnt = libusb_get_device_list(*ctx, devs);

    // does it return 0 if no USB device is found?
    if(cnt < 0) {
        DEBUG_PRINT(("No USB devices found.\n"));
    } else {
        DEBUG_PRINT(("%zu devices found.\n", cnt));
    }

    return cnt;
}

libusb_device_handle* get_first_printer(libusb_context **ctx) {
    libusb_device_handle *handle;
    libusb_device **device_list;
    struct libusb_device_descriptor desc;
    int retval;
    size_t i, device_count;

    device_list = NULL;
    handle = NULL;

    device_count = get_device_list(ctx, &device_list);

    for(i = 0; i < device_count; ++i) {
        retval = libusb_get_device_descriptor(device_list[i], &desc);

        if(retval >= 0) {
            DEBUG_PRINT(("%zu. %04x:%04x\n", i+1, desc.idVendor, 
                        desc.idProduct));
            if(desc.idVendor == 0x29c9) {
                DEBUG_PRINT(("Printer found! %04x:%04x\n", desc.idVendor, 
                            desc.idProduct));
                libusb_open(device_list[i], &handle);
                break;
            }
        } else {
            DEBUG_PRINT(("An error has occured obtaining a device " 
                        "descriptor.\n"));
        }
    }

    if(handle == NULL) {
        DEBUG_PRINT(("No BEEVERYCREATIVE printer was found\n"));
    } else {

        if(libusb_kernel_driver_active(handle, 0) == 1) {
            DEBUG_PRINT(("Kernel driver is active, attempting to "
                        "deattach...\n"));

            if(libusb_detach_kernel_driver(handle, 0) == 0) {
                DEBUG_PRINT(("Kernel driver deattached successfully\n"));
            }
        }

        if(libusb_claim_interface(handle, 0) >= 0) {
            DEBUG_PRINT(("Interface claimed successfully\n"));
        } else {
            DEBUG_PRINT(("Error while attempting to claim interface 0\n"));
        }
    }

    if(device_list != NULL) {
        libusb_free_device_list(device_list, 1);
        device_list = NULL;
    }

    return handle;
}

int send_message(libusb_device_handle *handle, unsigned char *message, 
        int message_size, int timeout) {
    int retval, bytes_sent;

    retval = libusb_bulk_transfer(handle, (LIBUSB_ENDPOINT_OUT | 
                BEETHEFIRST_ENDPOINT_OUT), message, message_size, &bytes_sent,
            timeout);

    if(retval == 0) {
        DEBUG_PRINT(("Message sent successfully, %d bytes were sent.\n", 
                    bytes_sent));
    } else {
        DEBUG_PRINT(("(Sending) %s (%d): %s\n", libusb_error_name(retval), 
                    retval, libusb_strerror(retval)));
    }

    return bytes_sent;
}

int receive_message(libusb_device_handle *handle, 
        unsigned char *message_buffer, int expected_size, int timeout) {
    unsigned char *temp_buffer;
    int retval, bytes_received, total_bytes_received;

    temp_buffer = NULL;
    bytes_received = 1;
    total_bytes_received = 0;

    while(bytes_received > 0) {
        temp_buffer = malloc(expected_size);

        retval = libusb_bulk_transfer(handle, (LIBUSB_ENDPOINT_IN | BEETHEFIRST_ENDPOINT_IN), temp_buffer, expected_size, &bytes_received, timeout);

        if(retval == 0) {
            DEBUG_PRINT(("Message received successfully, %d bytes were "
                        "received.\n", bytes_received));
            temp_buffer[bytes_received] = '\0';
            memcpy(&message_buffer[total_bytes_received], temp_buffer, 
                    bytes_received);
            total_bytes_received += bytes_received;
        } else {
            DEBUG_PRINT(("(Receiving) %s (%d): %s\n", libusb_error_name(retval), 
                        retval, libusb_strerror(retval)));
        }

        free(temp_buffer);
        temp_buffer = NULL;
    }

    message_buffer[total_bytes_received] = '\0';

    if(temp_buffer != NULL) {
        free(temp_buffer);
    }

    return total_bytes_received;
}
