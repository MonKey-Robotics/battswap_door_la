#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h> // For usleep

void writeToFile(const std::string &path, const std::string &value) {
    std::ofstream file(path);
    if (!file) {
        std::cerr << "Error writing to " << path << std::endl;
        return;
    }
    file << value;
}

int main() {
    const std::string gpioPin = "4"; // Replace with your GPIO pin number
    const std::string basePath = "/sys/class/gpio/gpio" + gpioPin;

    // Export the GPIO pin
    writeToFile("/sys/class/gpio/export", gpioPin);

    // Set direction to output
    writeToFile(basePath + "/direction", "out");

    // Turn GPIO pin ON
    writeToFile(basePath + "/value", "1");
    std::cout << "Relay is ON" << std::endl;
    usleep(5000000); // 5 seconds delay

    // Turn GPIO pin OFF
    writeToFile(basePath + "/value", "0");
    std::cout << "Relay is OFF" << std::endl;

    // Unexport the GPIO pin
    writeToFile("/sys/class/gpio/unexport", gpioPin);

    return 0;
}

// RUN COMMANDS
// g++ gpio_control.cpp -o gpio_control
// sudo ./gpio_control
