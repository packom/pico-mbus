#include <string.h>

#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/binary_info.h>
#include <mbus/mbus.h>

#include "config.h"

extern int init_slaves(mbus_handle *); // mbus-serial-request-data.c

static int debug = 0;

int main()
{
    mbus_handle *handle;
    long baudrate;
    char *device;
    int address;
    int ret;
    bool context_created = false;
    bool connected = false;
    bool power_on = false;
    bool results[MBUS_MAX_PRIMARY_SLAVES+1];
    int positive_count, collision_count;
    char *addr_str;
    char *xml_result = NULL;
    mbus_frame reply;
    mbus_frame_data reply_data;

    // Set binary information for this image (see https://github.com/raspberrypi/picotool#readme)
    bi_decl(bi_program_description("Query an M-Bus address for device information"));
    bi_decl(bi_program_url("https://github.com/packom/pico-mbus"));
    bi_decl(bi_program_version_string("Build date and time: " __DATE__ " " __TIME__));
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_UART_RX_PIN, PICO_DEFAULT_UART_TX_PIN, GPIO_FUNC_UART));
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_UART_TX_PIN, PICO_DEFAULT_UART_RX_PIN, GPIO_FUNC_UART));

    // Initialize stdio, including USB UART
    stdio_init_all();
    printf("USB Serial: Initialized\n");
    sleep_ms(PICO_MBUS_INIT_SLEEP);
    printf("------------\n");
    printf("M-Bus: Start\n");

    // Set up args used elsewhere
    debug = PICO_MBUS_DEBUG;
    baudrate = PICO_MBUS_BAUDRATE;
    addr_str = PICO_MBUS_ADDRESS_STRING;
    memset((void *)&reply, 0, sizeof(reply));
    memset((void *)&reply_data, 0, sizeof(reply_data));
    printf("M-Bus: Query data from device at address: %s\n", PICO_MBUS_ADDRESS_STRING);
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
        printf("M-Bus: Fatal error - invalid UART %d\n", PICO_MBUS_UART);
        device = "";
    }

START_LABEL:
    // Turn on M-Bus Power
    printf("M-Bus: Turn bus power on\n");
    gpio_init(PICO_MBUS_POWER_PIN);
    gpio_set_dir(PICO_MBUS_POWER_PIN, GPIO_OUT);
    gpio_put(PICO_MBUS_POWER_PIN, 1);
    power_on = true;

    printf("M-Bus: Pause for %dms\n", PICO_MBUS_POWER_ON_POST_DELAY);
    sleep_ms(PICO_MBUS_POWER_ON_POST_DELAY);

    printf("M-Bus: Create serial context\n");
    if ((handle = mbus_context_serial(device)) == NULL)
    {
        printf("M-Bus: Failed to create serial context: %s\n",  mbus_error_str());
        goto END_LABEL;
    }
    context_created = true;

    if (debug)
    {
        printf("M-Bus: Register send and receive event debug handlers\n");
        mbus_register_send_event(handle, &mbus_dump_send_event);
        mbus_register_recv_event(handle, &mbus_dump_recv_event);
    }

    printf("M-Bus: Connect to serial\n");
    if (mbus_connect(handle) == -1)
    {
        printf("M-Bus: Failed to connect: %s\n", mbus_error_str());
        goto END_LABEL;
    }
    connected = true;

    printf("M-Bus: Initialize slaves\n");
    if (init_slaves(handle) == 0)
    {
        printf("M-Bus: Failed to initialize slaves\n");
        goto END_LABEL;
    }

    if (mbus_is_secondary_address(addr_str))
    {
        // secondary addressing

        int ret;

        ret = mbus_select_secondary_address(handle, addr_str);

        if (ret == MBUS_PROBE_COLLISION)
        {
            printf("M-Bus: Error: The address mask [%s] matches more than one device\n", addr_str);
            goto END_LABEL;
        }
        else if (ret == MBUS_PROBE_NOTHING)
        {
            printf("M-Bus: Error: The selected secondary address does not match any device [%s]\n", addr_str);
            goto END_LABEL;
        }
        else if (ret == MBUS_PROBE_ERROR)
        {
            printf("M-Bus: Error: Failed to select secondary address [%s]\n", addr_str);
            goto END_LABEL;
        }
        // else MBUS_PROBE_SINGLE
        
        address = MBUS_ADDRESS_NETWORK_LAYER;
        printf("M-Bus: Secondary address: %s\n");
    }
    else
    {
        // primary addressing
        address = atoi(addr_str);
        printf("M-Bus: Primary address: %d\n", address);
    }

    if (mbus_send_request_frame(handle, address) == -1)
    {
        printf("M-Bus: Failed to send request frame\n");
        goto END_LABEL;
    }

    if (mbus_recv_frame(handle, &reply) != MBUS_RECV_RESULT_OK)
    {
        printf("M-Bus: Failed to receive response frame\n");
        goto END_LABEL;
    }

    if (debug)
    {
        mbus_frame_print(&reply);
    }

    if (mbus_frame_data_parse(&reply, &reply_data) == -1)
    {
        printf("M-Bus: Data parse error\n");
        goto END_LABEL;
    }

    if ((xml_result = mbus_frame_data_xml(&reply_data)) == NULL)
    {
        printf("M-Bus: Failed to generate XML representation of data: %s\n", mbus_error_str());
        goto END_LABEL;
    }

    printf("M-Bus: Query complete - results:\n");
    printf("%s", xml_result);

END_LABEL:

    if (xml_result)
    {
        free(xml_result);
        xml_result = NULL;
    }
    if (reply_data.data_var.record)
    {
        mbus_data_record_free(reply_data.data_var.record);
    }
    memset((void *)&reply, 0, sizeof(reply));
    memset((void *)&reply_data, 0, sizeof(reply_data));

    if (connected)
    {
        printf("M-Bus: Disconnect\n");
        mbus_disconnect(handle);
    }
    if (context_created)
    {
        printf("M-Bus: Free context\n");
        mbus_context_free(handle);
    }
    if (power_on)
    {
        printf("M-Bus: Turn bus power off\n");
        gpio_put(PICO_MBUS_POWER_PIN, 0);
    }
    printf("M-Bus: Pause for %dms before requerying\n", PICO_MBUS_SCAN_RETRY_TIMER);
    sleep_ms(PICO_MBUS_SCAN_RETRY_TIMER);

    printf("------------\n");

    goto START_LABEL;
}