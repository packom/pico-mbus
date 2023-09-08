//
// Add definitions via CMakeLists.txt
//

// PICO_MBUS_INIT_SLEEP - ms to sleep after boot before starting, to give time to e.g. connect to the USB serial port
// PICO_MBUS_DEBUG - 0 for no debug loggin, 1 for debug logging
// PICO_MBUS_BAUDRATE - valid M-Bus baurates, e.g. 2400
// PICO_MBUS_UART - Which UART to use for M-Bus comms.  Must not be the same as PICO_DEFAULT_UART
// PICO_MBUS_UART_TX_PIN - Must not be the same as PICO_DEFAULT_UART_TX_PIN
// PICO_MBUS_UART_RX_PIN - Must not be the same as PICO_DEFAULT_UART_RX_PIN
// PICO_MBUS_UART_DATA_BITS - Number of data bits for M-Bus UART, usually 8
// PICO_MBUS_UART_STOP_BITS - Number of stop bits for M-Bus UART, usually 1
// PICO_MBUS_UART_PARITY - Parity for M-Bus UART, usually UART_PARITY_EVEN
// PICO_MBUS_POWER_PIN - GPIO to use to turn M-Bus power on and off.  Pin 14 maps to Raspberry Pi BCM GPIO 26 using https://thepihut.com/products/raspberry-pi-pico-hat-expansion?variant=38176445071555
// PICO_MBUS_POWER_ON_POST_DELAY - How long in ms to wait after turning on M-Bus power before doing anything else
// PICO_MBUS_SEARCH_RETRY - Number of search transmission retries, from 0 to 9
// PICO_MBUS_SCAN_RETRY_TIMER - Length of time in ms to wait between rescans of the bus
// PICO_MBUS_RECV_DELAY - Number of ms to wait before attempting to receive data
// MBUS_SERIAL_DEBUG - Set to 1 for enable serial debug logs (these are very verbose)

