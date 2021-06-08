# DDCMP Framer software

The `firmware` subdirectory contains the software for the DDCMP framer.

## Loading the software

Plug a micro-USB to USB cable into the Raspberry Pico USB connector (at the top edge of the board) and plug the other end into a suitable PC or Mac.  You should see a file system (removable storage device) appear named `RPI-RP2`.  If you are reprogramming an already programmed board, rather than doing the initial load onto a board newly delivered from the factory, you will need to hold down the pushbutton on the Raspberry Pico board while you're connecting it to the PC, this forces the boot ROM to enter software load mode.

Copy, by drag and drop or a suitable shell command, the file `ddcmp.uf2` to that file system.  The code will be written into on-board flash and will start.  This results in the abrupt disappearance of the `RPI-RP2` file system; your PC may object to this and display some sort of warning message.  Ignore that message.

## DDCMP Framer operation

When running, the framer will appear as an Ethernet device, in whatever way your host machine operating system handles this.  You can recognize it by the fact that its MAC address is always aa:00:03:04:05:06.  It should appear within a few seconds after you plug in the framer.  When it does, the device is ready for use.

You will need to communicate with the framer using Ethernet packets to tell it what to do.  Sufficiently recent versions of DECnet/Python (pydecnet) and SIMH (TBD) can do this; refer to the documentation that comes with these packages.  You can also write your own controlling software, using the information in file `api.md`.

### Self-test

If J4 is installed, you can access an internal self-test feature by installing a jumper on that header before connecting the board to power.  This will set internal loopback mode at 1 Mb/s and a test packet generator that sends 500  byte packets continuously.  Normal operation is shown by all three LEDs lighting up solid.  Every 1000 packets the self-test will send a status API message to the connected host; you can use host software to receive and display these, or capture them with a tool such as tcpdump or wireshark.  To terminate the test, simply unplug the USB cable and remove the jumper.

## Building the software

A pre-built binary `ddcmp.uf2` is supplied on Github.  You can build your own, modified if desired.  Install the Raspberry Pico SDK using the instructions given in [Getting started with Raspberry Pi Pico](https://datasheets.raspberrypi.org/pico/getting-started-with-pico.pdf "Getting started document").  Be sure to set environment variable `PICO_SDK_PATH` to the root directory of the SDK.  You will also need the `cmake` tool.

With the SDK installed, go to the `firmware` directory and execute:

    cmake .
    make

This will build all the necessary components and produce a loadable binary, `ddcmp.uf2`.  Load it into the board using the instructions given above.

## Options

There is only one build option: change `#define DEBUG 0` to `#define DEBUG 1` to enable debug output to the UART.  Plug in a 3.3 volt UART at J1 (note the orientation: pin 1, black wire, is marked by a dot).  The UART is set for 115,200 baud, 8 bits, no parity.  Debug messages appear in the source code as DPRINTF statements.  You can define DEBUG to be 2, this also enables the detailed debug messages (DDPRINTF), but that is very verbose and will significantly slow down operation of the device.
