/// ? Serial Fun
/// Serial ports are represented by files within our operating system. These files
/// usually pop-up in /dev/, and begins with the tty*. Thats all cool and fun, but
/// how can we use serial ports ?.
/// Well, because everything is a file in Unix systems, to write to a serial port
/// all we have to do is write to th file, and as you guessed, to read from a serial
/// port, all we have to do is read from the file. This allows us to send and receive
/// data. But because serial communications has it protocols, we need to setup some
/// parameters before we can read and write anything meaningful.  We do this by 
/// configuring a special tty config struct.

/// RESOURCES:
/// https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/


/// C LIB HEADERS
#include <stdio.h>
#include <string.h>
/// LINUX HEADERS
#include <fcntl.h>       /// Contains file controls like O_RWDR
#include <termios.h>     /// contains POSIX terminal control definitions
#include <unistd.h>      /// write(), read(), close()
#include <threads.h>
#include "checks.h"


#define START_BYTE 0x02  // STX
#define STOP_BYTE  0x03  // EXT
#define ESC_BYTE   0x10  // ESC


int main() {
    /// Before we can mess around with a serial port, we first have to open it
    const char* port = "/dev/ttyACM0";
    int serial_port = open(port, O_RDWR);
    CHECK_OPEN(serial_port > 0);

    /// After successfully getting a handle to the serial port file, we can now
    /// open and configure it, we need to create a new termios struct, and then
    /// write the existing configuration of the serial port to it, before modifying
    /// these parameters and then saving.
    struct termios tty;
    CHECK((tcgetattr(serial_port, &tty) == 0),"tcgetattr()");

    /// Now we can modify tty's settings as needed
    /// PARENB (Parity)
    /// If this bit is set, generation and detection of the parity bit is enabled.
    /// Most serial communications do not use a parity bit, s
    tty.c_cflag &= ~PARENB;         // Clear parity bit.
    tty.c_cflag &= ~CSTOPB;         // one stop bit.
    tty.c_cflag &= ~CSIZE;          // clear all the size bits.
    tty.c_cflag |=  CS8;            // 8 bits per byte.
    tty.c_cflag |=  CREAD | CLOCAL; //Turn on read & ignore ctrl lines.
    tty.c_lflag &= ~ICANON;         // disable canonical mode.
    tty.c_lflag &= ~ECHO;           // disable echo.
    tty.c_lflag &= ~ECHOE;          // disable erasure.
    tty.c_lflag &= ~ECHONL;         // disable new-line echo.
    tty.c_lflag &= ~ISIG;           // disable interpretation of INTR, QUIT and SUSP.
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // No software flow control.
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling of received bytes
    tty.c_oflag &= ~OPOST;           // Prevent special interpretation of output bytes.
    tty.c_oflag &= ~ONLCR;           // Prevent conversion of newline to carriage return/line feed.
    tty.c_cc[VMIN] = 1;              // Read at least 1 byte
    tty.c_cc[VTIME] = 0;             // No timeout.

    cfsetispeed(&tty, B9600);        // set in baud rate.
    cfsetospeed(&tty, B9600);        // set out baud rate.

    /// We can now save our new serial port setting s and check for any errors:
    CHECK(tcsetattr(serial_port, TCSANOW, &tty) == 0, "tcsetattr()");
    
    /// At this point, we can now read and write to the serial port :)
    /// to write to the serial port, we use write()
    /// reading is done with read(), but we have to provide a buffer for our os to write
    /// the data into.
    

    /// for now lets just keep sending a true value, we will use this to toggle the on board led of the uno.

    for (;;) {
        // write(serial_port, "1", 1);
        // printf("TOGGLE ON-BOARD LED...\n");
        // sleep(2);
        // write(serial_port, "0", 1);
        // printf("TOGGLE ON-BOARD LED...\n");
        // sleep(2);
        char msg[] = {START_BYTE, 2, 'O', 'N', STOP_BYTE};
        char msg1[] = {START_BYTE, 3, 'O', 'F', 'F', STOP_BYTE};

        write(serial_port, msg, sizeof(msg));
        sleep(1);

        // char read_buf[64];
        // memset(read_buf, 0, sizeof(read_buf));

        // int num_bytes = read(serial_port, read_buf, sizeof(read_buf));
        // if (num_bytes > 0) {
        //     printf("Received (%d bytes): ", num_bytes);
        //     for (int i = 0; i < num_bytes; i++) {
        //         printf("%c", read_buf[i]);
        //     }
        //     printf("\n");
        // }
    }