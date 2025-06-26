#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/// ? We need to set up our flags
typedef enum {
    START_BYTE_FLAG  = 0,
    LENGTH_BYTE_FLAG = 1,
    STOP_BYTE_FLAG   = 2,
    POST_CMD_FLAG    = 3,
    GET_CMD_FLAG     = 4,
    ESC_CMD_FLAG     = 5,
} COMMUNICATION_FLAGS;



/// ? We need to select special bytes to represent START_BYTE and STOP_BYTE
#define START_BYTE 0x02  // STX
#define STOP_BYTE  0x03  // EXT
#define ESC_BYTE   0x10  // ESC


/// ? The maximum length of a message is 254 bytes, remember we need a null
/// ? terminator '\0', making it 255.
char comms_buf[255] = {0};     


/// ? Flags used for communication
uint8_t comms_flags = 0;   /// ! Clear this.


/// ? Command length
uint8_t command_length  = 0;
uint8_t current_length = 0;


/// @brief Interpretes our cmd ans executes it
void interprete_cmd() {
    printf("CMD PAYLOAD: %s\n", comms_buf);
}


int main() {
    /// ? MSG EXAMPLE
    char msg[] = {START_BYTE, 100, 'e', 'c', 'h', 's', 'a', 'g', 'e', STOP_BYTE};
    for (size_t idx = 0; idx < 10; ++idx) {
        char data = msg[idx];

        switch (data) {
            /// * 1. Set Start flag
            /// * 2. Reset current_length on START_BYTE
            case START_BYTE: {
                comms_flags |= (1 << START_BYTE);
                current_length = 0;
                memset(comms_buf, '\0', sizeof(comms_buf));
                break;
            }
            
            /// * 1. Clear all flags
            /// * 2. Call interprete_cmd
            case STOP_BYTE: {
                comms_buf[current_length] = '\0';
                interprete_cmd();
                comms_flags = 0;
                current_length = 0;
                memset(comms_buf, '\0', sizeof(comms_buf));
                break;
            }

            /// * 1. Clear all flags
            /// * 2. Discard all received data.
            /// * 3. call interprete_cmd.
            case ESC_BYTE: {
                comms_flags = 0;
                memset(comms_buf, '\0', sizeof(comms_buf));
                break;
            }

            /// * 1. Attach received data to the buffer.
            /// * 2. Keep track of msg length so far
            default: {
                /// ? Check if its msg length data
                /// ? We know its msg length if the start flag is set and the len flag is NOT SET
                if (comms_flags & (1 << START_BYTE) && !(comms_flags & (1 << LENGTH_BYTE_FLAG))) {
                    /// ? Store msg length
                    command_length = data;
                    /// ? Set LENGTH_BYTE_FLAG
                    comms_flags |= 1 << LENGTH_BYTE_FLAG;
                    printf("msg length is: %d\n", data);
                    break;
                }
                /// ? Payload 
                if (comms_flags & (1 << START_BYTE) && comms_flags & (1 << LENGTH_BYTE_FLAG)) {
                    comms_buf[current_length] = data;
                    current_length++;
                    break;
                }
            }
    }
}
    
}