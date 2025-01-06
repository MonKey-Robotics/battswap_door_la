#include <libserial/SerialPort.h>
#include <iostream>
#include <thread> // For sleep

using namespace LibSerial;
int main() {
    SerialPort serialPort;

    try {
        serialPort.Open("/dev/ttyUSB0");
        serialPort.SetBaudRate(BaudRate::BAUD_9600);

        // Turn RTS ON
        serialPort.SetRTS(true);
        std::cout << "Relay ON" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));

        // Turn RTS OFF
        serialPort.SetRTS(false);
        std::cout << "Relay OFF" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));

        // Turn RTS ON
        serialPort.SetRTS(true);
        std::cout << "Relay ON" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));

        // Turn RTS OFF
        serialPort.SetRTS(false);
        std::cout << "Relay OFF" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));

        serialPort.Close();
    } catch (const std::exception& e) {
        std::cerr << "Failed to open the serial port!" << std::endl;
        return 1;
    }
    return 0;
}


//RUN THIS --> g++ -o test2 test2.cpp -lserial
//THEN THIS --> ./test2

