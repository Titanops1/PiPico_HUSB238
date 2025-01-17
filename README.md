# HUSB238 - Raspberry Pi Pico Library

**HUSB238** is a custom C library for the Raspberry Pi Pico, designed to facilitate communication and control of the HUSB238 chip.

## Features
- **Initialization**: Easy setup for communication with the HUSB238 chip.
- **Data Transfer**: Supports I²C-based control and data retrieval.
- **Configuration Functions**: Parameter customization and chip monitoring.

## Requirements
- CMake (version 3.13 or higher)
- Pico SDK
- A Raspberry Pi Pico or compatible device

## Installation

### 1. Clone this repository

git clone https://github.com/Titanops1/husb238.git

### 2. Include in your project

Integrate the library into your project structure as follows:

- /pico_project
- ├── CMakeLists.txt       # Main CMake file of your project
- ├── src
- │   ├── main.c           # Main program
- ├── build                # Build folder

Edit your project’s CMakeLists.txt to include the library:
```c
cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(pico_project)

pico_sdk_init()

# Add the library
add_subdirectory(/path/to/husb238 ${CMAKE_BINARY_DIR}/husb238)

# Main program
add_executable(pico_project src/main.c)

# Link the library
target_link_libraries(pico_project husb238 pico_stdlib hardware_i2c)

# Enable standard output
pico_enable_stdio_usb(pico_project 1)
pico_enable_stdio_uart(pico_project 0)
```
### 3. Build your project

Build your project with the included library:
- mkdir build
- cd build
- cmake ..
- make

## Usage

In your code, include the library as follows:
```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "husb238.h"

int main() {
    stdio_init_all();

    // I2C Initialization
    i2c_inst_t *i2c_instance = i2c0;
    const uint sda_pin = 4;
    const uint scl_pin = 5;

    i2c_init(i2c_instance, 100 * 1000); // 100 kHz
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);

    // Example of Lib usage
    husb238_init(i2c_instance);

    husb238_getSupportedVoltages();

    while (true) {
        husb238_selectPD(PD_SRC_9V);
        husb238_requestPD();
        pdresponse = husb238_getPDResponse();
        src_voltage = husb238_getPDSrcVoltage();
        src_current = husb238_getPDSrcCurrent();
    }

    return 0;
}
```