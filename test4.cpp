#include <gpiod.h>
#include <iostream>
#include <unistd.h> // For usleep

#define CHIP_NAME "/dev/gpiochip0" // Adjust based on your system
#define GPIO_PIN 1 // Replace with your GPIO pin number
#define GPIO_PIN 2 // Replace with your GPIO pin number

int main() {
    gpiod_chip *chip = gpiod_chip_open(CHIP_NAME);
    if (!chip) {
        std::cerr << "Failed to open GPIO chip" << std::endl;
        return -1;
    }

    gpiod_line *line = gpiod_chip_get_line(chip, GPIO_PIN);
    if (!line) {
        std::cerr << "Failed to get GPIO line" << std::endl;
        gpiod_chip_close(chip);
        return -1;
    }

    if (gpiod_line_request_output(line, "gpio_control", 0) < 0) {
        std::cerr << "Failed to request line as output" << std::endl;
        gpiod_chip_close(chip);
        return -1;
    }

    // Turn GPIO ON
    gpiod_line_set_value(line, 1);
    std::cout << "Relay is ON" << std::endl;
    usleep(5000000); // 5 seconds

    // Turn GPIO OFF
    gpiod_line_set_value(line, 0);
    std::cout << "Relay is OFF" << std::endl;

    // Release resources
    gpiod_line_release(line);
    gpiod_chip_close(chip);

    return 0;
}


// RUN COMMANDS //
// g++ -o gpio_control gpio_control.cpp -lgpiod
// sudo ./gpio_control
