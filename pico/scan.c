#include <string.h>

#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/binary_info.h>
#include <mbus/mbus.h>

#include "config.h"

extern int ping_address(mbus_handle *, mbus_frame *, int); // mbus-serial-scan.c

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

    // Set binary information for this image (see https://github.com/raspberrypi/picotool#readme)
    bi_decl(bi_program_description("Performs a scan of primary M-Bus addresses looking for attached slaves"));
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

    printf("M-Bus: Set search retries value to %d\n", PICO_MBUS_SEARCH_RETRY);
    if (mbus_context_set_option(handle, MBUS_OPTION_MAX_SEARCH_RETRY, PICO_MBUS_SEARCH_RETRY) == -1)
    {
        printf("M-Bus: Failed to set retry count to %d\n", PICO_MBUS_SEARCH_RETRY);
        goto END_LABEL;
    }

    printf("M-Bus: Scanning primary addresses:\n");
    positive_count = 0;
    collision_count = 0;
    for (address = 0; address <= MBUS_MAX_PRIMARY_SLAVES; address++)
    {
        results[address] = false;
        mbus_frame reply;

        ret = ping_address(handle, &reply, address);

        if (ret == MBUS_RECV_RESULT_TIMEOUT)
        {
            continue;
        }

        if (ret == MBUS_RECV_RESULT_INVALID)
        {
            /* check for more data (collision) */
            mbus_purge_frames(handle);
            printf("M-Bus: Collision at address %d\n", address);
            collision_count++;
            continue;
        }

        if (mbus_frame_type(&reply) == MBUS_FRAME_TYPE_ACK)
        {
            /* check for more data (collision) */
            if (mbus_purge_frames(handle))
            {
                printf("M-Bus: Collision at address %d\n", address);
                continue;
            }

            printf("Found an M-Bus device at address %d\n", address);
            positive_count++;
            results[address] = true;
        }
    }

    printf("M-Bus: Scan complete - results:\n");
    printf("       Slaves found:    %d\n", positive_count);
    printf("       Collisions:      %d\n", collision_count);
    printf("       Addresses found: ");
    for (address = 0; address <= MBUS_MAX_PRIMARY_SLAVES; address++)
    {
        if (results[address])
        {
            printf("%d ", address);
        }
    }
    printf("\n");

END_LABEL:
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
    printf("M-Bus: Pause for %dms before rescanning\n", PICO_MBUS_SCAN_RETRY_TIMER);
    sleep_ms(PICO_MBUS_SCAN_RETRY_TIMER);

    printf("------------\n");

    goto START_LABEL;
}

    
