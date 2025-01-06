#include <gpiod.h>
#include <iostream>
#include <unistd.h> // For usleep

#define CHIP_NAME "/dev/gpiochip0" // Adjust based on your system
#define GPIO_PIN_1 1              // Replace with your GPIO pin number
#define GPIO_PIN_2 2              // Replace with your GPIO pin number

int main() {
    // Open the GPIO chip
    gpiod_chip *chip = gpiod_chip_open(CHIP_NAME);
    if (!chip) {
        std::cerr << "Failed to open GPIO chip" << std::endl;
        return -1;
    }

    // Get GPIO lines
    gpiod_line *line1 = gpiod_chip_get_line(chip, GPIO_PIN_1);
    gpiod_line *line2 = gpiod_chip_get_line(chip, GPIO_PIN_2);

    if (!line1 || !line2) {
        std::cerr << "Failed to get GPIO lines" << std::endl;
        gpiod_chip_close(chip);
        return -1;
    }

    // Request GPIO lines as outputs
    if (gpiod_line_request_output(line1, "gpio_control_1", 0) < 0) {
        std::cerr << "Failed to request GPIO line 1 as output" << std::endl;
        gpiod_chip_close(chip);
        return -1;
    }

    if (gpiod_line_request_output(line2, "gpio_control_2", 0) < 0) {
        std::cerr << "Failed to request GPIO line 2 as output" << std::endl;
        gpiod_chip_close(chip);
        return -1;
    }

    // Control GPIO lines individually
    // Turn GPIO_PIN_1 ON, GPIO_PIN_2 OFF
    gpiod_line_set_value(line1, 1);
    gpiod_line_set_value(line2, 1);
    std::cout << "GPIO 1 is ON, GPIO 2 is ON" << std::endl;
    usleep(5000000); // Wait 5 seconds

    // Turn GPIO_PIN_1 OFF, GPIO_PIN_2 ON
    gpiod_line_set_value(line1, 0);
    gpiod_line_set_value(line2, 1);
    std::cout << "GPIO 1 is OFF, GPIO 2 is ON" << std::endl;
    usleep(5000000); // Wait 5 seconds

    // Turn both GPIO_PIN_1 and GPIO_PIN_2 OFF
    // gpiod_line_set_value(line1, 0);
    // gpiod_line_set_value(line2, 1);
    // std::cout << "Both GPIOs are OFF" << std::endl;

    // Release resources
    gpiod_line_release(line1);
    gpiod_line_release(line2);
    gpiod_chip_close(chip);

    return 0;
}
