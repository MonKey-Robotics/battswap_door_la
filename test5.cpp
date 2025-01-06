#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstdint>

#define GPIO_BASE_ADDR  0xXXXX  // Replace with actual base address of the GPIO controller
#define DO_REGISTER     0x31   // Offset for DO register
#define DI_REGISTER     0x30   // Offset for DI register

#define GPIO_MEM_SIZE   0x1000 // Adjust based on datasheet memory map

class GPIOController {
private:
    volatile uint8_t* gpio_base;
    int mem_fd;

public:
    GPIOController() : gpio_base(nullptr), mem_fd(-1) {}

    ~GPIOController() {
        if (gpio_base) {
            munmap(const_cast<uint8_t*>(gpio_base), GPIO_MEM_SIZE);
        }
        if (mem_fd != -1) {
            close(mem_fd);
        }
    }

    bool initialize() {
        mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
        if (mem_fd == -1) {
            std::cerr << "Failed to open /dev/mem\n";
            return false;
        }

        gpio_base = reinterpret_cast<uint8_t*>(mmap(nullptr, GPIO_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, GPIO_BASE_ADDR));
        if (gpio_base == MAP_FAILED) {
            std::cerr << "Failed to mmap GPIO memory\n";
            return false;
        }
        return true;
    }

    void writeDO(uint8_t value) {
        *(gpio_base + DO_REGISTER) = value;
    }

    uint8_t readDI() {
        return *(gpio_base + DI_REGISTER);
    }
};

int main() {
    GPIOController gpio;
    if (!gpio.initialize()) {
        return 1;
    }

    // Example: Set GPIO1 and GPIO2 high (bits 0 and 1)
    gpio.writeDO(0b00000011);

    // Example: Read the status of DI pins
    uint8_t di_status = gpio.readDI();
    std::cout << "DI Status: " << std::bitset<8>(di_status) << "\n";

    return 0;
}
