#include "usb.h"

int init_libusb(libusb_context **ctx) {
    int ret_val;

    ret_val = libusb_init(ctx);

    // TODO: interpret error given
    if(ret_val < 0) {
        printf("Error initializing libusb.\n");
    } else {
        libusb_set_debug(*ctx, 3);
    }

    return ret_val;
}

int get_device_list(libusb_context **ctx, libusb_device ***devs) {
    size_t cnt;
    
    cnt = libusb_get_device_list(*ctx, devs);

    // TODO: interpret error given
    // does it return 0 if no USB device is found?
    if(cnt < 0) {
        printf("No USB devices found.\n");
    } else {
        printf("%zu devices found.\n", cnt);
    }

    return cnt;
}

libusb_device_handle* get_first_printer(libusb_device **device_list, 
        size_t device_list_size) {
    libusb_device_handle *handle;
    struct libusb_device_descriptor desc;
    int retval;
    size_t i;

    handle = NULL;

    for(i = 0; i < device_list_size; ++i) {
        retval = libusb_get_device_descriptor(device_list[i], &desc);

        if(retval >= 0) {
            printf("idVendor = %x\n", desc.idVendor);
            if(desc.idVendor == 0x29c9) {
                printf("printer found!\n");
                libusb_open(device_list[i], &handle);
                break;
            }
        } else {
            printf("An error has occured obtaining a device descriptor\n");
        }
    }

    if(handle == NULL) {
        printf("No BEEVERYCREATIVE printer was found\n");
        return NULL;
    }

    libusb_free_device_list(device_list, 1);

    if(libusb_kernel_driver_active(handle, 0) == 1) {
        printf("Kernel driver is active, attempting do deattach...\n");

        if(libusb_detach_kernel_driver(handle, 0) == 0) {
            printf("Kernel driver deattached successfully\n");
        }
    }

    if(libusb_claim_interface(handle, 0) >= 0) {
        printf("Interface claimed successfully\n");
    } else {
        printf("Error while attempting to claim interface 0\n");
    }

    return handle;
}

