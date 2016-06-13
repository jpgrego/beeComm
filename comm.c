#include "usb.h"

void test_message(libusb_device_handle *handle);

int main(int argc, char** argv) {
    libusb_device **devs;
    libusb_device_handle *beeprinter;
    libusb_context *ctx;
    size_t device_count;

    devs = NULL;
    beeprinter = NULL;
    ctx = NULL;

    // initialize libusb and set debug level
   if(init_libusb(&ctx) < 0) {
        return 1;
    }

    // get a list containing all the devices connected to the system
    device_count = get_device_list(&ctx, &devs);
    if(device_count < 0) {
        return 1;
    }

    // obtain a pointer to the first BEEVERYCREATIVE printer found
    beeprinter = get_first_printer(devs, device_count);

    if(beeprinter != NULL) {
        printf("beeprinter is not NULL!\n");
        test_message(beeprinter);
    } else {
        printf("beeprinter is NULL :(\n");
    }


    if(beeprinter != NULL) {
        printf("Releasing interface and closing printer\n");
        libusb_release_interface(beeprinter, 0);
        libusb_close(beeprinter);
    }
    
    if(ctx != NULL) {
        printf("Exiting libusb\n");
        libusb_exit(ctx);
    }
    return 0;
}

void test_message(libusb_device_handle *handle) {
    unsigned char *data;
    int retval, bytes_sent, bytes_received;

    data = malloc(5);
    //data = "M115";
    //data_size = strlen(data);
    data[0] = 'M';
    data[1] = '1';
    data[2] = '1';
    data[3] = '5';
    data[4] = '\n';

    retval = libusb_bulk_transfer(handle, (LIBUSB_ENDPOINT_OUT | 5), data, 5, &bytes_sent, 0);


    if(retval == 0) {
        printf("Message sent successfully, %d bytes were sent\n", bytes_sent);
    } else {
        printf("An error has occurred while sending the message\n");
    }

    data = realloc(data, 128);

    retval = libusb_bulk_transfer(handle, (LIBUSB_ENDPOINT_IN | 2), data, 128, &bytes_received, 0);

    if(retval == 0) {
        printf("%d bytes received!\n", bytes_received);
        data[bytes_received] = '\0';
        printf("%s\n", data);
    } else {
        printf("An error has occured while receiving the answer\n");
    }

    free(data);

}
