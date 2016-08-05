#include "usb.h"
#include <unistd.h>

#define INPUT_BUFFER_SIZE 100
#define SEND_TIMEOUT 3000
#define RECEIVE_TIMEOUT 10
#define MESSAGE_SIZE 2048

int main(int argc, char** argv) {
    libusb_device_handle *beeprinter;
    libusb_context *ctx;
    size_t transferred_bytes, message_size;
    char *buffer;
    unsigned char *send_buffer, *answer;

    beeprinter = NULL;
    ctx = NULL;
    buffer = NULL;
    send_buffer = NULL;
    answer = NULL;

    // initialize libusb and set debug level
   if(init_libusb(&ctx) < 0) {
        return 1;
    }
 
    // obtain a pointer to the first BEEVERYCREATIVE printer found
    printf("Attempting to find a BEEVERYCREATIVE printer...\n");
    while(beeprinter == NULL) {
        beeprinter = get_first_printer(&ctx);
        
        if(beeprinter == NULL) {
            sleep(0.2);
        }
    }

    printf("Printer has been found!\n\n");
    printf("Initiating console. Type \"exit\" to finish.\n\n");

    while(1) {
        printf("> ");

        buffer = malloc(INPUT_BUFFER_SIZE);
        fgets(buffer, INPUT_BUFFER_SIZE, stdin);
        fflush(stdin);

        // remove trailing newline character
        /*
        if((pos = strchr(buffer, '\0')) != NULL) {
            *pos = '\0';
        }
        */

        if(strcmp(buffer, "exit\n") == 0) {
            break;
        }

        message_size = strlen(buffer);
        send_buffer = malloc(message_size);
        memcpy(send_buffer, buffer, message_size);
        transferred_bytes = send_message(beeprinter, send_buffer, message_size, 
                SEND_TIMEOUT);
        
        if(transferred_bytes > 0) {
            answer = malloc(MESSAGE_SIZE);
            transferred_bytes = receive_message(beeprinter, answer, 
                    MESSAGE_SIZE, RECEIVE_TIMEOUT); 

            if(transferred_bytes > 0) {
                printf("%s\n", answer);
            } else {
                printf("(no response)\n");
            }
        } else {
            printf("(error sending message)\n");
        }

        if(strcmp(buffer, "M630\n") == 0 || strcmp(buffer, "M609\n") == 0) {
            sleep(2);
            close_device(beeprinter);
            beeprinter = NULL;
            while(beeprinter == NULL) {
                beeprinter = get_first_printer(&ctx);
        
                if(beeprinter == NULL) {
                    sleep(0.2);
                }
            }
        }

        free(buffer);
        free(answer);
        free(send_buffer);
        buffer = NULL;
        answer = NULL;
        send_buffer = NULL;

    }

//cleanup:
    close_device(beeprinter);
    close_libusb(&ctx);
    beeprinter = NULL;
    ctx = NULL;

    if(buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }

    return 0;
}


