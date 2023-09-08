cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(pico-mbus C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_definitions(-DRPI_PICO 
  -DPICO_DEFAULT_UART=1
  -DPICO_DEFAULT_UART_TX_PIN=4
  -DPICO_DEFAULT_UART_RX_PIN=5
  -DPICO_BOOTSEL_VIA_DOUBLE_RESET_TIMEOUT_MS=1000
  -DPICO_MBUS_INIT_SLEEP=2000
  -DPICO_MBUS_DEBUG=0
  -DPICO_MBUS_BAUDRATE=2400
  -DPICO_MBUS_UART=0
  -DPICO_MBUS_UART_TX_PIN=0
  -DPICO_MBUS_UART_RX_PIN=1
  -DPICO_MBUS_UART_DATA_BITS=8
  -DPICO_MBUS_UART_STOP_BITS=1
  -DPICO_MBUS_UART_PARITY=UART_PARITY_EVEN
  -DPICO_MBUS_POWER_PIN=14
  -DPICO_MBUS_POWER_ON_POST_DELAY=1000
  -DPICO_MBUS_SEARCH_RETRY=0
  -DPICO_MBUS_SCAN_RETRY_TIMER=10000
  -DPICO_MBUS_RECV_DELAY=100
)

add_library(libmbus
  libmbus/mbus/mbus-protocol.c
  libmbus/mbus/mbus-protocol-aux.c
  libmbus/mbus-serial.c
  libmbus/bin/mbus-serial-scan.c
)
add_executable(mbus-scan
  pico/scan.c)

include_directories(${CMAKE_SOURCE_DIR})

target_link_libraries(libmbus pico_stdlib hardware_uart)
target_link_libraries(mbus-scan
  libmbus
  pico_bootsel_via_double_reset
)

pico_add_extra_outputs(mbus-scan)
pico_enable_stdio_usb(mbus-scan 1)
pico_enable_stdio_uart(mbus-scan 0)