#include <string.h>

#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/binary_info.h>
#include <mbus/mbus.h>

#include "config.h"

static int debug = 0;

int main()
{
    mbus_handle *handle;
    long baudrate;
    char *device;
    bool context_created = false;
    bool connected = false;
    char *buf;
    int buf_len;
    uart_inst_t *uart;
    int sleep_time;
    char rx_buf[81];
    int rx_buf_len = 81;
    int read;
    mbus_serial_data *serial_data;

    // Set binary information for this image (see https://github.com/raspberrypi/picotool#readme)
    bi_decl(bi_program_description("Test the serial implementation"));
    bi_decl(bi_program_url("https://github.com/packom/pico-mbus"));
    bi_decl(bi_program_version_string("Build date and time: " __DATE__ " " __TIME__));
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_UART_RX_PIN, PICO_DEFAULT_UART_TX_PIN, GPIO_FUNC_UART));
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_UART_TX_PIN, PICO_DEFAULT_UART_RX_PIN, GPIO_FUNC_UART));

    // Initialize stdio, including USB UART
    stdio_init_all();
    printf("USB Serial: Initialized\n");
    sleep_ms(PICO_MBUS_INIT_SLEEP);
    printf("------------\n");
    printf("Test: Start\n");

    // Set up args used elsewhere
    debug = PICO_MBUS_DEBUG;
    baudrate = PICO_MBUS_BAUDRATE;
    if (PICO_MBUS_UART == 0)
    {
        device = "UART0";
    }
    else if (PICO_MBUS_UART == 1)
    {
        device = "UART1";
    }
    else
    {
        printf("Test: Fatal error - invalid UART %d\n", PICO_MBUS_UART);
        device = "";
    }

START_LABEL:

    printf("Test: Create serial context\n");
    if ((handle = mbus_context_serial(device)) == NULL)
    {
        printf("Test: Failed to create serial context: %s\n",  mbus_error_str());
        goto END_LABEL;
    }
    context_created = true;

    if (debug)
    {
        printf("Test: Register send and receive event debug handlers\n");
        mbus_register_send_event(handle, &mbus_dump_send_event);
        mbus_register_recv_event(handle, &mbus_dump_recv_event);
    }

    printf("Test: Connect to serial\n");
    if (mbus_connect(handle) == -1)
    {
        printf("Test: Failed to connect: %s\n", mbus_error_str());
        goto END_LABEL;
    }
    connected = true;

    uart = mbus_serial_pico_get_uart_from_fd(handle->fd);

    buf = "Send me some data back:";
    buf_len = strlen(buf);
    printf("Test: Send some data: %s\n", buf);
    uart_write_blocking(uart, buf, buf_len);

    sleep_time = 20000;
    printf("Test: Pause for %dms to receive up to %d bytes of data\n", sleep_time, rx_buf_len-1);
    sleep_ms(sleep_time);
    serial_data = (mbus_serial_data *)handle->auxdata;
    do
    {
        printf("Test: M-Bus rx buffer: data_len: %d first: %d next: %d\n",
            serial_data->data_len,
            serial_data->first,
            serial_data->next);
        read = mbus_serial_pico_read(handle, rx_buf, rx_buf_len-1);
        rx_buf[read] = 0;
        printf("Test: Received %d bytes of data: %s\n", read, rx_buf);
    } while (read == (rx_buf_len-1));
    

END_LABEL:

    if (connected)
    {
        printf("Test: Disconnect\n");
        mbus_disconnect(handle);
    }
    if (context_created)
    {
        printf("Test: Free context\n");
        mbus_context_free(handle);
    }
    sleep_time = 10000;
    printf("Test: Pause for %dms before restarting\n", sleep_time);
    sleep_ms(sleep_time);

    printf("------------\n");    

    goto START_LABEL;
}



