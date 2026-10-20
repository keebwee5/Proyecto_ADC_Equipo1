cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(ADC_LDR C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(ADC_LDR
    src/main.c
    src/adc_regs.c
    src/adc_sdk.c
    src/verifica_mapa.c
)

target_include_directories(ADC_LDR PRIVATE src)
target_compile_options(ADC_LDR PRIVATE -Wall -Wextra)

target_link_libraries(ADC_LDR
    pico_stdlib
    hardware_adc
    hardware_gpio
)

pico_enable_stdio_usb(ADC_LDR 1)
pico_enable_stdio_uart(ADC_LDR 0)

pico_add_extra_outputs(ADC_LDR)
