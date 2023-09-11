//
// Add definitions via CMakeLists.txt
//

//
// pico-mbus #defines
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
// PICO_MBUS_ADDRESS_STRING - Address of device to communicate with, either primary or secondary address
// PICO_MBUS_RX_BUF_LEN - The size of the RX buffer - should be substantially greater than expected M-Bus frame size.  The Pico has 264kbytes of RAM, so 1K should be reasonable.
// PICO_MBUS_RX_WAIT_TIME_MS - Amount of time to be prepared to wait to read a specified number of bytes 
// PICO MBUS_RX_BUF_FULL_OVERWRITE - Defines behaviour if our RX buffer is full and we receive a byte.  0 means don't overwrite the first stored byte (through away the one we're going to read instead), 1 means overwrite the first byte (i.e. new first byte will be the second byte).  Default is 0 (discard the next byte read).
//

// 
// libmbus #defines
//
// MBUS_SERIAL_DEBUG - Set to 1 for enable serial debug logs (these are very verbose)
//