# pico-mbus

An M-Bus implementation for the Raspberry Pi Pico.

## Getting the Pico SDK

If you need install the Pico SDK, you can use [pico-build](https://github.com/piersfinlayson/pico-build).

## Checking out and building

```
git clone https://github.com/packom/pico-mbus
cd pico-mbus
git submodule update --init --recursive
cmake .
make -j 4
```

## Changing configuration

Most of the configuration settings are controlled via #defines in the CMakeLists.txt file, using the _add_definitions_ call.

You can see a description of the settings in pico/config.h.

If you change any of the settings, you will need to re-run:

```
cmake .
make -j 4
```

## Installing

With a Pico plugged in to a USB port, in BOOTSEL mode, run:

```
./flash-pico.sh <device> <image> 
```

For example, if your Pico is /dev/sdh, and you want to flash mbus-scan, run:

```
./flash-pico.sh sdh mbus-scan.uf2
```

## Monitoring serial output

By default USB serial support is configured, using UART1 (UART0 is used to communicate with the M-Bus Master device), so you should be able to connect a terminal to the Pico via USB once the image is flashed.  For example:

```
minicom -o -D /dev/ttyACM0 
```

## M-Bus Master Support

This has been tested with the [M-Bus Master Hat](https://www.packom.net/product/m-bus-master-hat/) for Raspberry Pis, attached using a Pico->Pi Hat converter board.  In particular:
* Pico GP0 is connected to RPi Hat GPIO14
* Pico GP1 is connected to RPi Hat GPIO15
* Pico GP14 is mapped to RPi Hat GPIO26
* 3V3, 5V and GND must also be connected to the Hat. 

The is no need to connect the RPi Hat ID_SD and ID_SC pins, and identification of the Hat by the Pico is not supported.

## Programs

mbus-scan
* Scans the primary addresses 0-250 looking for M-Bus slaves
* Once the scan is completed, prints the addresses of discovered slaves via USB serial

mbus-query
* Queries device at the configured address (PICO_MBUS_ADDRESS_STRING) and outputs retrieved data, formatted as XML, to USB serial

test-serial
* Used to check the rx ring buffer implementation
* Sends a serial via serial and waits for some data to be sent back

