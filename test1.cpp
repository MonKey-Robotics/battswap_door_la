#include <iostream>
#include <fcntl.h>     // For open()
#include <unistd.h>    // For close(), sleep()
#include <termios.h>   // For termios structure
#include <sys/ioctl.h> // For ioctl()

int main() {
    const char* port = "/dev/ttyUSB0"; // Specify the serial port

    //O_RDWR: Open in read and write mode.
    //O_NOCTTY: Prevent the serial port from becoming the controlling terminal of the process.
    int fd = open(port, O_RDWR | O_NOCTTY);

    if (fd == -1) {
        std::cerr << "Error: Unable to open port " << port << std::endl;
        return 1;
    }

    // Ensure the port is in a proper state
    // Retrieves the current configuration of the serial port and stores it in the tty structure.
    // Returns 0 on success, or a non-zero value on failure.
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "Error: Unable to get port attributes" << std::endl;
        close(fd);
        return 1;
    }

    // Set baud rate to 9600
    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    // Configure port settings
    tty.c_cflag |= (CLOCAL | CREAD);  // Enable the receiver and set local mode
    tty.c_cflag &= ~CSIZE;           // Clear current data size setting
    tty.c_cflag |= CS8;              // Set 8 data bits
    tty.c_cflag &= ~PARENB;          // No parity
    tty.c_cflag &= ~CSTOPB;          // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;         // Disable hardware flow control

    // Apply the settings
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "Error: Unable to set port attributes" << std::endl;
        close(fd);
        return 1;
    }

    // Relay ON (set RTS)
    int status;
    ioctl(fd, TIOCMGET, &status);     // Get current modem status
    status |= TIOCM_RTS;              // Set RTS high
    ioctl(fd, TIOCMSET, &status);     // Update modem status
    std::cout << "Relay ON" << std::endl;

    sleep(5); // Wait for 5 seconds

    // Relay OFF (clear RTS)
    ioctl(fd, TIOCMGET, &status);     // Get current modem status
    status &= ~TIOCM_RTS;             // Set RTS low
    ioctl(fd, TIOCMSET, &status);     // Update modem status
    std::cout << "Relay OFF" << std::endl;

    sleep(5); // Wait for 5 seconds

    // Close the port
    close(fd);
    std::cout << "Port closed" << std::endl;

    return 0;
}
