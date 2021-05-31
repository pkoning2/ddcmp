cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)


project(ddcmp)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_library(ddcmp_libs INTERFACE)

target_link_libraries(ddcmp_libs INTERFACE
    hardware_clocks
    hardware_gpio
    hardware_dma
    hardware_pio
    hardware_timer
    pico_multicore
    pico_platform
    pico_stdlib
    pico_stdio_uart
    pico_printf
)

target_include_directories(ddcmp_libs INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${PICO_SDK_PATH}/lib/tinyusb/lib/networking)
target_link_libraries(ddcmp_libs INTERFACE
    tinyusb_device
    pico_stdio_usb)

add_executable(ddcmp ddcmp.c usb_descriptors.c)
pico_generate_pio_header(ddcmp ${CMAKE_CURRENT_LIST_DIR}/ddcmp.pio)

target_link_libraries(ddcmp ddcmp_libs)
pico_set_program_name(ddcmp "DDCMP Framer")

# Option to make the code run in RAM.  That doesn't appear to have any
# useful performance benefit so don't bother with it.
#pico_set_binary_type(ddcmp copy_to_ram)

pico_add_extra_outputs(ddcmp)
