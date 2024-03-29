// DDCMP Framer
//
// This is a USB-connected device that connects to DDCMP synchronous
// link devices, either RS-232 or DEC "integral modem" coax links,
// such as the DMC-11.
//
// Copyright (c) 2021, Paul Koning
//
// Please read file LICENSE for the license that applies to this project.


// Define this as 1 to send debug output to the debug UART, 0 to turn
// off UART use.
#define DEBUG 0

// Define this as 0 to turn off padding of frames going to the host.
// For maximal compatibility with various Ethernet drivers we normally
// pad frames so they are at least 60 bytes (Ethernet minimum, not
// counting CRC).  While USB does not need this, SIMH does.
//
// Note that padding is always allowed but never required on frames
// from host to framer.
//
// Padding has a noticeable impact on performance when dealing with
// rapid fire short frames (such as control frames).  For example, the
// unit test TestRaw.test_concat_ctl works at the full 1 Mb/s speed
// with padding off, but can only do 500 kb/s if padding is turned on.
#define PADRUNT 1

// Define this to 1 (default) for 8 bit transmit FIFO, 0 for 32 bit
// transmit FIFO.  For standard speeds, 8 bit FIFO is more than
// adequate.
#define TX8 1

#define __packed 
#define __aligned(x)

#include <string.h>

#include "bsp/board.h"
#include "tusb.h"

#include "ddcmp.pio.h"
#include <hardware/pio_instructions.h>
#include <hardware/timer.h>
#include "hardware/structs/sio.h"
#include "hardware/regs/sio.h"
#include <hardware/dma.h>
#include <pico/multicore.h>
#include <hardware/pll.h>
#include <hardware/clocks.h>
#include <pico/unique_id.h>

#define VERSION "DDCMP Framer V1.0"

#define MB asm volatile ("" : : : "memory")

#if PADRUNT
#define MINE(x) ((x) > 60) ? (x) : 60
#else
#define MINE(x) (x)
#endif

#if DEBUG
// Debugging, if enabled, uses the UART at the default pins (pin 1 and
// 2, GPIO 0 and GPIO 1).
#include <stdio.h>
#include <pico/stdio_uart.h>

#define DPRINTF printf
#else
#define DPRINTF(fmt...) // empty
#endif
#if DEBUG > 1
#define DDPRINTF DPRINTF
#else
#define DDPRINTF(fmt...) // empty
#endif

// PIO and state machine index assignments for the PIO state machines
// we use.  Note that the main transmit and receive state machines all
// have to be in the same PIO since IRQ, in spite of what the data
// sheet implies, is local to a given PIO and these state machines use
// IRQ to signal the receive machine to shift another bit.
#define RXPIO pio0
#define RXSM  0
#define IMBITPIO pio0
#define IMBITSM  1
#define TXPIO pio0
#define TXSM  2
#define FREQPIO  pio1
#define FREQSM   0
#define WDOGPIO  pio1
#define WDOGSM   1
// The two LED pulse stretcher SMs share the same program
#define BLINKPIO pio1
#define RXLEDSM  3
#define TXLEDSM  2

// Load addresses for these
uint16_t rxtop, txtop, imbittop, blinktop, freqtop;

// Jumps back to top
uint16_t rxsm_jmptop;
uint16_t blinksm_jmptop;
uint16_t freqsm_jmptop;

// Core 1 related state
volatile bool core1_active = false;
volatile int avg_period = 0;

// Receive (and more generally, the device) enabled state.
volatile bool rx_enabled = false;

// Last reported SYN status
bool syn_status = false;

// Selftest mode active
bool bist = false;

// Pin numbers (GPIOn numbers, to be precise)
// Clock/data pins
#define TXCLKIN 12      // Transmit clock input from modem
#define TXDATA  15      // Transmit data output
#define RXDATA  13      // RS232 receive data input
#define RXCLKIN 14      // Receive clock input from modem, must be RXDATA+1
#define CLKOUT  11      // RS232 clock output (RX/TX both) to DTE
#define IMTXDATA 17     // Integral modem transmit data output
#define IMRXDATA 19     // Integral modem receive data input
// Enable pins
#define RS232EN 10      // RS-232 output enable
#define IMEN    18      // Integral Modem enable
// Loopback test control
#define LOOPTEST 2      // Pull low to force power on selftest
// LED pin assignments
#define SYNLED   26     // In-sync indicator LED output
#define RXLED    27     // Packet received LED
#define ACTLED   25     // Device is active (RPico built-in LED)
#define TXLED    28     // Packet sent LED
// No-connect pins.  These are used when a signal needs to get from
// one state machine to another.  That applies to integral modem
// receive data (from demodulator), as well as to the loopback case.
// These pins are not connected to anything on the PC board, but they
// are enabled (both input and output circuits) so they have to be
// reserved for the purpose and configured as needed just like other
// pins.  IMSTROBE is an engineering test signal; the integral modem
// demodulator state machine generates a rising edge at the point
// where it samples the incoming waveform (nominally in the middle of
// the second half of the bit cell).
#define RXRECDATA 8     // Integral modem recovered receive bit stream
#define IMSTROBE  9     // Integral modem receive strobe (RXRECDATA + 1)
#define LCPIN     6     // Loopback clock pin
#define LDPIN     7     // Loopback data pin
#define CLOCKOK   16    // Bit clock ok (from watchdog state machine)

// These are pins we use as output; they are explicitly driven to 0 at
// initialization and ddcmp stop.
const uint8_t opins[] = {
    TXDATA,
    CLKOUT,
    IMTXDATA,
    RS232EN,
    IMEN,
    SYNLED,
    RXLED,
    ACTLED,
    TXLED,
    RXRECDATA,
    IMSTROBE,
    LCPIN,
    LDPIN,
    CLOCKOK
};

// DDCMP character codes
#define SOH 0201        // SOH - start of data message
#define ENQ 0005        // ENQ - start of control message
#define DLE 0220        // DLE - start of maintenance message
#define SYN 0226        // SYN - synchronization code
#define DEL 0377        // DEL - pad after message trailer

#define DC1 0021        // DC1 - used here for command/status, not DDCMP

#define SYN4 0x96969696U
#define RXLIMIT (1498 - 2) // Max received total frame size to host
#define RDMAX (RXLIMIT - 8 - 2) // Max receive DDCMP data length
#define TXLIMIT 1498    // Max transmit total frame size from host
#define TDMAX (TXLIMIT - 8) // Max transmit DDCMP data length
#define MAXLEN (RDMAX < TDMAX ? RDMAX : TDMAX) // Max DDCMP payload
#define RDATA (MAXLEN + 8 + 2) // Frame space in rings
#define ETH_HDR 14      // Length of Ethernet header
#define RBUF_OFF (ETH_HDR + 4) // Rx buffer offset to start of DDCMP frame
#define SYN_CNT 8       // Count of SYN bytes before tx frame start
#if TX8
#define SYN_CNT_V3 10   // Ditto, but in DMC11-AL V3 compatibility mode
#else
#define SYN_CNT_V3 12   // Ditto, but in DMC11-AL V3 compatibility mode
#endif

// Buffer counts.  The receive buffer count can be increased if
// desired, though there does not seem to be a good reason for doing
// so.  The transmit buffer count must NOT be increased (8 is the max)
// because there needs to be an available entry in the CPU0 -> CPU1
// FIFO for each transmit buffer.
#define RBUFS 8         // Number of buffers in receive ring
#define TBUFS 8         // Number of buffers in transmit ring

// Operating mode flags.  BIST is not normally requested from the
// host, but is generated internally if the LOOPTEST pin is pulled low
// at powerup.
#define INT_MODEM       1
#define RS232_LOCAL_CLK 2
#define INT_LOOPBACK    4
#define BIST            8
#define SPLIT           16
#define DDCMP3          32  // DDCMP V3.03 (DMC-11) compatibility
#define RAW_WAVEFORM    64  // Integral modem test, send raw waveforms

// DDCMP framing state machine state codes
enum ddcmp_state
{
    PKT_START,          // Looking for start of packet
    HEADER,             // Processing header
    PAYLOAD,            // Processing payload
    DEL1,               // Checking for optional trailing DEL
    FLUSH               // Flushing receive FIFO for resync
};

// DDCMP frame buffer status codes
enum ddcmp_status
{
    DDCMP_OK,           // Good packet
    DDCMP_HCRC,         // Header CRC error
    DDCMP_CRC,          // Payload CRC error
    DDCMP_LONG,         // Payload too long
    DDCMP_LOC,          // Loss of carrier in mid-frame
    BUF_EMPTY           // Buffer waiting to be filled
};

// DDCMP frame buffer layout (framer to host)
struct ddcmp_rxframe
{
    uint8_t bufhdr[ETH_HDR]; // room for "Ethernet" header
    uint16_t data_len;      // DDCMP payload length
    volatile uint16_t stat; // Status (ddcmp_status code)
    uint8_t data[RDATA];    // DDCMP frame data (header + payload)
};

// Receive ring
struct ddcmp_rxframe rbuf_ring[RBUFS];
volatile int rbuf_empty = 0;
volatile int rbuf_fill = 0;

// DDCMP frame buffer layout (host to framer, transmit ring entry)
struct ddcmp_txframe
{
    volatile uint32_t data_len; // Total length (including SYN and DEL)
    uint8_t *data_start;    // Starting address (syn or syn + 2 or 4)
    uint8_t syn[SYN_CNT_V3]; // SYN bytes before frame start
    uint8_t data[RDATA];    // DDCMP frame data (header + payload)
    uint8_t del;            // DEL after the CRC
};

// Transmit ring
struct ddcmp_txframe tbuf_ring[TBUFS];
volatile int tbuf_empty = 0;
volatile int tbuf_fill = 0;

// Set to true in the transmit handler if it has no room for another
// transmit, checked in the main loop to re-request the transmit
// buffer from USB if the count now permits transmitting more.
volatile bool tx_full = false;

// Set to true if we're in DMC11 mode using integral modem, i.e.,
// talking to a DMC11-AL.
bool dmc_syn_al = false;

// Set to true if we're in DMC11 mode, any link type.
bool dmc = false;

// True if we're in raw waveform mode.
bool raw_wave = false;

// DMA channel number
int txdma;
// DMA channel mask
uint txdmask;
// DMA channel CSR pointer
volatile dma_channel_hw_t *txcsr;

// Default buffer header (Ethernet header) for messages from framer to
// host.  The host interface address is aa-00-03-xx-yy-zz where the
// last 24 bits are random values generated from the board unique ID.
// The framer appears as an Ethernet device on that LAN with address
// one larger than the host interface address.
static uint8_t default_hdr[RBUF_OFF] = {
    0xaa, 0x00, 0x03, 0x00, 0x00, 0x00,         // Dest:   aa-00-03-xx-yy-zz
    0xaa, 0x00, 0x03, 0x00, 0x00, 0x00,         // Source
    0x60, 0x06,                                 // Prot:   60-06 (customer)
    0, 0,                                       // Payload length
    BUF_EMPTY, 0x00                             // Status
};

// This is the USB Ethernet interface address.
//
// PLEASE NOTE: In the standard tinyusb library this is declared as
// "extern const uint8_t [6]" which we can't accept because the low
// order bits have to be set at initialization time.  You will need to
// edit src/class/net/net_device.h to remove the "const" from that
// declaration, otherwise the compiler will complain.
uint8_t tud_network_mac_address[6] = {
    0xaa, 0x00, 0x03, 0x00, 0x00, 0x00         // ifaddr: aa-00-03-xx-yy-zz
};

// Command message.  This starts like a data message, except that the
// header byte is DC1 (0x11) rather than one of the three DDCMP start
// of header codes.
struct command_msg_t
{
    uint8_t dc1;
    uint8_t cmd;
    uint16_t mflags __attribute__ ((packed));
    uint32_t speed __attribute__ ((packed));
    uint32_t txspeed __attribute__ ((packed)); // Optional: split speed
};

enum ddcmp_action
{
    REQ_STATUS,         // Get device status
    REQ_START,          // Start DDCMP engine
    REQ_STOP,           // Stop DDCMP engine
    REQ_RAWTX           // Raw transmit
};

// Status message.  This starts like a data message, except that the
// header byte is DC1 (0x11) rather than one of the three DDCMP start
// of header codes.
// Note that things must be kept naturally aligned to avoid padding.
struct status_msg_t
{
    uint8_t bufhdr[ETH_HDR];        // room for "Ethernet" header
    uint16_t data_len;              // Length of Ethernet payload
    uint16_t stat;                  // Status (ddcmp_status code)
    uint8_t dc1;
    uint8_t on;                     // "on" flags
    uint16_t mflags;
    uint16_t sdusize;               // Max DDCMP frame payload
    uint32_t speed;
    uint32_t txspeed;
    uint32_t rxframes;
    uint32_t rxbytes;
    uint32_t txframes;
    uint32_t txbytes;
    uint32_t hcrc_err;
    uint32_t crc_err;
    uint32_t len_err;
    uint32_t nobuf_err;
    uint32_t last_cmd_sts;          // Response code from last command
    uint32_t freq;                  // Measured frequency
    char version[sizeof (VERSION)];
};

#define ON_ACT 1
#define ON_SYN 2
#define ON_CLKOK 4

enum last_cmd_status_t
{
    CMD_OK = 0,             // Command was accepted
    CMD_BAD_MSG,            // Command message too short
    CMD_UNKNOWN,            // Unknown command code
    CMD_ACTIVE,             // Start command when already active
    CMD_BAD_SPEED,          // Speed is out of range
    TX_BAD_LENGTH,          // Transmit frame bad length
    TX_OFF,                 // Transmit request when device is off
    CMD_BAD_MODE,           // Invalid start mode flags
    TX_WHEN_RAW,            // Transmit during raw waveform mode
};

struct status_msg_t status_msg;
bool send_status = false;

// 8 bits at a time CRC table, from PyDECnet crc.py
// crc16 polynomial x^16 + x^15 + x^2 + 1 (0x8005) CCITT LSB 
static const uint16_t crc16_table[256] = {
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
    0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
    0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
    0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
    0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
    0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
    0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
    0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
    0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
    0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
    0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
    0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
    0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
    0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
    0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
    0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
    0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
    0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
    0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
    0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
    0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
    0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
    0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
    0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
    0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
    0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
};

static inline uint16_t update_crc (uint16_t crc, uint8_t b)
{
    crc = (crc >> 8) ^ crc16_table[b ^ (crc & 0xff)];
    return crc;
}

// Set the PIO receive state machine back to its start to make it go
// back to searching for SYN.
static inline void syn_search (void)
{
    pio_sm_exec (RXPIO, RXSM, rxsm_jmptop);
}

static inline void rxblink (void)
{
    pio_sm_exec (BLINKPIO, RXLEDSM, blinksm_jmptop);
}

static inline void txblink (void)
{
    pio_sm_exec (BLINKPIO, TXLEDSM, blinksm_jmptop);
}

static inline void buf_done (enum ddcmp_status stat)
{
    DDPRINTF ("buffer %d (%d) done, status %d\n", rbuf_fill, rbuf_empty, stat);
    MB;
    rbuf_ring[rbuf_fill].stat = stat;
    rbuf_fill = (rbuf_fill + 1) % RBUFS;
    rxblink ();
}

// For some reason these aren't standard functions.
static void gpio_set_drive (int gpio, int strength)
{
    // Set drive strength bits
    hw_write_masked(&padsbank0_hw->io[gpio],
                    strength << PADS_BANK0_GPIO0_DRIVE_LSB,
                    PADS_BANK0_GPIO0_DRIVE_BITS);
}

static void gpio_set_slew (int gpio, int fast)
{
    // Set slew rate bit
    hw_write_masked(&padsbank0_hw->io[gpio],
                    fast << PADS_BANK0_GPIO0_SLEWFAST_LSB,
                    PADS_BANK0_GPIO0_SLEWFAST_BITS);
}

// CPU 1 polling loop.
//
// While CPU 0 is assigned to general data handling tasks and in
// particular the USB interaction, we have a need for hard real time
// low latency handling of the data streams from and to the PIO state
// machines.  For simplicity and reliability this is implemented as a
// polling loop.
//
// This loop is started when the framer is started by host request for
// the first time.  Since the PICO SDK does not offer a primitive for
// "stopping" CPU 1, the loop continues to run until the device is
// powered off, even if the framer is stopped.  The "rx_enabled" flag
// tells the polling loop whether the framer is on or off.
//
// The polling loop performs three tasks.  They are, in priority order:
//
// 1. Receive FIFO handling.  Received bytes (including SYN bytes
//    between packets, once sync has been established) are sent to the
//    receive state machine FIFO 4 bytes at a time.  The FIFO has 8
//    entries, i.e., the FIFO capacity is 32 bytes.  The polling loop
//    reads entries from the FIFO so long as it is not empty,
//    assembling DDCMP frames according to the frame construction
//    rules, checking CRC as it goes.  Frames are buffered into the
//    next available receive buffer in the receive ring (if there is a
//    free one).  When the frame is complete, it is marked as such by
//    setting the receive status in the ring entry and advancing the
//    ring index.  This allows the CPU 0 USB service code to see the
//    completed buffer and deliver it to the host via USB.
//
// 2. Transmit DMA control.  Unlike receive processing, transmit
//    frames are prepared largely by the host, with CRC supplied by
//    CPU 0 as it copies the frame into a free transmit ring entry.
//    The ring entry address is then written to the CPU0->CPU1 FIFO,
//    indicating it is ready to be transmitted.  The actual delivery
//    of frame data is via DMA to the transmit PIO state machine FIFO.
//    The polling loop checks whether the DMA engine is idle.  If yes,
//    and a transmit was in progress, it is marked as complete by
//    setting the length field of its transmit ring entry to zero, and
//    the current transmit pointer is cleared.  Next, if there is an
//    entry in the CPU0->CPU1 FIFO, it is read; that is the next
//    transmit ring entry to send.  The polling loop loads buffer
//    address and length into the DMA engine, which starts the
//    transfer, and remembers the ring entry pointer to do completion
//    handling as described above.
//
// 3. Frequency measurement data collection.  The frequency PIO state
//    machine measures the interval between edges in the received data
//    stream, more precisely across 32 edges, and reports that cycle
//    count by writing that into its FIFO.  The polling loop reads
//    these FIFO entries and computes a weight-32 weighted average of
//    the reported values.  That average is reported on request by CPU
//    0 in the status frame it sends in response to commands.
void ddcmp_cpu1 (void)
{
    int i, bytes;
    bool ctrl;
    uint32_t rword;
    enum ddcmp_state ds = PKT_START;
    uint16_t crc;
    struct ddcmp_rxframe *frame;
    struct ddcmp_txframe *txframe = NULL;
    uint8_t *bp;

    DPRINTF ("This is core 1, active now\n");
    
    while (1)
    {
        // Look for another word, but if the receive PIO state machine
        // is halted, go back to "searching for start of frame" state.
        // That way things recover properly if the device is stopped
        // in mid-frame.
        while (1)
        {
            if (rx_enabled)
            {
                if (!pio_sm_is_rx_fifo_empty (RXPIO, RXSM))
                {
                    rword = pio_sm_get (RXPIO, RXSM);
                    DDPRINTF ("%08x\n", rword);
                    break;
                }
                // Next priority after receive: transmit DMA
                // completion and startup.  We do this if DMA is not
                // busy. 
                if (!(txcsr->al1_ctrl & DMA_CH0_CTRL_TRIG_BUSY_BITS))
                {
                    if (txframe != NULL)
                    {
                        // Current frame pointer is set, mark that
                        // frame as done and clear the pointer.
                        txframe->data_len = 0;
                        txframe = NULL;
                    }
                    if (sio_hw->fifo_st & SIO_FIFO_ST_VLD_BITS)
                    {
                        // There is data (frame pointer) in the 0->1
                        // FIFO.  Get that pointer and start the DMA
                        // for that buffer.  It is now the current
                        // frame pointer.
                        txframe = (struct ddcmp_txframe *) (sio_hw->fifo_rd);
                        txcsr->al1_read_addr = (uint32_t) (txframe->data_start);
                        txcsr->al1_transfer_count_trig = txframe->data_len;
                    }
                }
                // Lower priority than data handling: gather up
                // frequency measurement data.
                if (!pio_sm_is_rx_fifo_empty (FREQPIO, FREQSM))
                {
                    // Get a sample from the frequency counter
                    int period = (-(int) pio_sm_get (FREQPIO, FREQSM)) << 5;

                    // Process sample only if bit clock is good
                    if (gpio_get (CLOCKOK))
                    {
                        int avg = avg_period;

                        if (avg)
                        {
                            // Update the weighted average (weight = 32).
                            avg = ((avg << 5) - avg + period) >> 5;
                            avg_period = avg;
                        }
                        else
                        {
                            avg_period = period;
                        }
                    }
                }
                // Check if bit clock is currently bad
                if (!gpio_get (CLOCKOK))
                {
                    // No clock.  If we're in mid packet, terminate
                    // that packet with a loss of clock error.
                    if (!(ds == DEL1 || ds == PKT_START || ds == FLUSH))
                    {
                        DPRINTF ("Loss of clock in mid-frame, state = %d\n", ds);
                        buf_done (DDCMP_LOC);
                        // TODO: count it?
                    }
                    if (gpio_get (SYNLED))
                    {
                        // We were in sync, force syn search
                        DPRINTF ("Loss of clock while in sync\n");
                        syn_search ();
                        ds = FLUSH;
                    }
                    // Restart the frequency measurement SM
                    pio_sm_exec (FREQPIO, FREQSM, freqsm_jmptop);
                }
            }
            else
            {
                // If not enabled, force the state to "searching for
                // start of packet".
                ds = PKT_START;
            }
        }
        if (rword == SYN4 && ds == FLUSH)
        {
            ds = PKT_START;
            continue;
        }
        for (i = 0; i < 4; i++)
        {
            uint8_t b = rword & 0xff;
            rword >>= 8;
            if (ds != PKT_START || b != SYN)
            {
                DDPRINTF ("S%d D0x%02x\n", ds, b);
            }
            switch (ds)
            {
            case DEL1:
                // Next state is always "looking for packet start"
                ds = PKT_START;
                if (b == DEL)
                {
                    break;
                }
                // FALL_THROUGH
            case PKT_START:
                if (b == SOH || b == ENQ || b == DLE)
                {
                    // Start of frame, see if we have a frame buffer
                    frame = &rbuf_ring[rbuf_fill];
                    if (frame->stat != BUF_EMPTY)
                    {
                        // Nope
                        DPRINTF ("no buffer available\n");
                        status_msg.nobuf_err++;
                        syn_search ();
                        ds = FLUSH;
                        break;
                    }
                    crc = 0;
                    bytes = frame->data_len = 8;
                    ctrl = b == ENQ;
                    ds = HEADER;
                    bp = frame->data;
                    // Fall through to store the start byte and
                    // accumulate CRC
                }
                else if (b == SYN)
                {
                    // Still in between frames
                    break;
                }
                else
                {
                    syn_search ();
                    ds = FLUSH;
                    break;
                }
                // FALL_THROUGH
            case HEADER:
            case PAYLOAD:
                *bp++ = b;
                crc = update_crc (crc, b);
                if (--bytes == 0)
                {
                    // End of header or payload
                    if (crc == 0)
                    {
                        // Good CRC
                        if (ctrl || ds == PAYLOAD)
                        {
                            // Good frame, count it
                            status_msg.rxframes++;
                            status_msg.rxbytes += frame->data_len;
                            buf_done (DDCMP_OK);
                            // Skip possible trailing DEL, then look
                            // for next frame
                            ds = DEL1;
                        }
                        else
                        {
                            ds = PAYLOAD;
                            // Find payload length plus 2 for CRC
                            bytes = 2 + (frame->data[1] |
                                         ((frame->data[2] & 0x3f) << 8));
                            if (bytes > MAXLEN + 2 || bytes == 2)
                            {
                                // Rather than go through extra
                                // trouble we'll classify too short
                                // (zero length field) along with too
                                // long.
                                buf_done (DDCMP_LONG);
                                status_msg.len_err++;
                                syn_search ();
                                ds = FLUSH;
                                break;
                            }
                            frame->data_len += bytes;
                        }
                    }
                    else
                    {
                        // Bad CRC, report it and resync if header CRC
                        if (ds == PAYLOAD)
                        {
                            status_msg.crc_err++;
                            buf_done (DDCMP_CRC);
                            ds = DEL1;
                        }
                        else
                        {
                            status_msg.hcrc_err++;
                            buf_done (DDCMP_HCRC);
                            syn_search ();
                            ds = FLUSH;
                        }
                    }
                }
                break;
            case FLUSH:
                break;
            }
        }
    }
}

// Set pio_sm_config clkdiv parameters given desired clock frequency.
// More precisely, we set the dividers to the nearest integer, to
// avoid the clock jitter associated with the use of fractional
// dividers.  For all the standard rates the error resulting from this
// is either 0 or quite small enough (less than 0.1%).
//
// Lifted, with some mods, from rp2_pio.c in micropython.
void sm_config_set_clkdiv_freq (pio_sm_config *c, int freq)
{
    // Frequency given in Hz, compute clkdiv from it.
    uint i, f;
    uint64_t q;

    q = ((uint64_t) clock_get_hz (clk_sys) * 256) / freq;
    
    i = q / 256;
    f = q & 0xff;

    // Round up
    if (f >= 128)
    {
        i++;
    }
    sm_config_set_clkdiv_int_frac (c, i, 0);
    DPRINTF ("Clock freq %d div %d\n", freq, i);
}

// PIO state machine setup handlers.  Each of these get the "default"
// config from the code generated by pioasm (which basically just
// handles the side_set parameters from the .side_set directive).  The
// other config parameters are then filled in here.
static pio_sm_config modem_clk_xmit_config (uint offset)
{
    pio_sm_config c;
    c = modem_clk_xmit_program_get_default_config (offset);
    txtop = offset;
    
    // Fast clock
    sm_config_set_clkdiv_int_frac (&c, 1, 0);
    // FIFO: threshold 8 (or 32), shift right, no autopull, joined
#if TX8
    sm_config_set_out_shift (&c, true, false, 8);
#else
    sm_config_set_out_shift (&c, true, false, 32);
#endif
    sm_config_set_fifo_join (&c, PIO_FIFO_JOIN_TX);
    // input: tx clock input pin
    sm_config_set_in_pins (&c, TXCLKIN);
    // output: tx data
    sm_config_set_out_pins (&c, TXDATA, 1);
    
    return c;
}

static pio_sm_config modem_clk_recv_config (uint offset)
{
    pio_sm_config c;
    c = modem_clk_recv_program_get_default_config (offset);
    rxtop = offset;
    
    // Fast clock
    sm_config_set_clkdiv_int_frac (&c, 1, 0);
    // FIFO: threshold 32, shift right, no autopush
    // Make sure join is off; it will be turned on as part of starting
    // the state machine.
    sm_config_set_in_shift (&c, true, false, 32);
    sm_config_set_fifo_join (&c, PIO_FIFO_JOIN_NONE);
    // input: rx data input pin (rx clock input pin is next after that)
    sm_config_set_in_pins (&c, RXDATA);
    // side set: "in sync" LED
    sm_config_set_sideset_pins (&c, SYNLED);
    
    return c;
}

static pio_sm_config local_clk_xmit_config (uint offset, uint speed,
                                            int clkpin, int txpin)
{
    pio_sm_config c;
    c = local_clk_xmit_program_get_default_config (offset);
    txtop = offset;

    // clock from bit rate
    sm_config_set_clkdiv_freq (&c, speed * 4);
    // FIFO: threshold 8 (or 32), shift right, no autopull, joined
#if TX8
    sm_config_set_out_shift (&c, true, false, 8);
#else
    sm_config_set_out_shift (&c, true, false, 32);
#endif
    sm_config_set_fifo_join (&c, PIO_FIFO_JOIN_TX);
    // output: tx data
    sm_config_set_out_pins (&c, txpin, 1);
    // side set: clock output pin
    sm_config_set_sideset_pins (&c, clkpin);
    
    return c;
}

static pio_sm_config inmodem_xmit_config (uint offset, uint speed, int txpin)
{
    pio_sm_config c;
    c = inmodem_xmit_program_get_default_config (offset);
    txtop = offset;
    
    // clock from bit rate
    sm_config_set_clkdiv_freq (&c, speed * 4);
    // FIFO: threshold 8 (or 32), shift right, no autopull, joined
#if TX8
    sm_config_set_out_shift (&c, true, false, 8);
#else
    sm_config_set_out_shift (&c, true, false, 32);
#endif
    sm_config_set_fifo_join (&c, PIO_FIFO_JOIN_TX);
    // output: integral modem tx data
    // side set: ditto
    sm_config_set_out_pins (&c, txpin, 1);
    sm_config_set_sideset_pins (&c, txpin);
    
    return c;
}

static pio_sm_config inmodem_rawxmit_config (uint offset, uint speed, int txpin)
{
    pio_sm_config c;
    c = inmodem_rawxmit_program_get_default_config (offset);
    txtop = offset;
    
    // clock from bit rate
    sm_config_set_clkdiv_freq (&c, speed * 4);
    // FIFO: threshold 8 (or 32), shift right, no autopull, joined
#if TX8
    sm_config_set_out_shift (&c, true, false, 8);
#else
    sm_config_set_out_shift (&c, true, false, 32);
#endif
    sm_config_set_fifo_join (&c, PIO_FIFO_JOIN_TX);
    // output: integral modem tx data
    // no side set
    sm_config_set_out_pins (&c, txpin, 1);
    
    return c;
}

static pio_sm_config local_clk_recv_config (uint offset, int rxpin)
{
    pio_sm_config c;
    c = local_clk_recv_program_get_default_config (offset);
    rxtop = offset;
    
    // Fast clock
    sm_config_set_clkdiv_int_frac (&c, 1, 0);
    // FIFO: threshold 32, shift right, no autopush
    // Make sure join is off; it will be turned on as part of starting
    // the state machine.
    sm_config_set_in_shift (&c, true, false, 32);
    sm_config_set_fifo_join (&c, PIO_FIFO_JOIN_NONE);
    // input: rx recovered bit stream pin (integral modem case) or rx
    // data input pin (rs232 case)
    sm_config_set_in_pins (&c, rxpin);
    // side set: "in sync" LED
    sm_config_set_sideset_pins (&c, SYNLED);
    
    return c;
}

static pio_sm_config inmodem_rxbit_config (uint offset, uint speed, int rxpin)
{
    pio_sm_config c;
    c = inmodem_rxbit_program_get_default_config (offset);
    imbittop = offset;
    
    // clock from bit rate
    sm_config_set_clkdiv_freq (&c, speed * 12);
    // input and jump pin: integral modem rx data
    sm_config_set_in_pins (&c, rxpin);
    sm_config_set_jmp_pin (&c, rxpin);
    // set: strobe signal
    sm_config_set_set_pins (&c, IMSTROBE, 1);
    // side set: integral modem recovered data and strobe
    sm_config_set_sideset_pins (&c, RXRECDATA);

    return c;
}

static pio_sm_config pulse_stretch_config (uint offset)
{
    pio_sm_config c;
    c = pulse_stretch_program_get_default_config (offset);
    blinktop = offset;
    blinksm_jmptop = pio_encode_jmp (offset);
    
    // clock: 2 kHz
    sm_config_set_clkdiv_freq (&c, 2000);
    // set: RX LED pin
    sm_config_set_set_pins (&c, RXLED, 1);

    return c;
}

static pio_sm_config freq_config (uint offset, int pin)
{
    pio_sm_config c;
    c = freq_program_get_default_config (offset);
    freqtop = offset;
    freqsm_jmptop = pio_encode_jmp (offset);
    sm_config_set_in_pins (&c, CLOCKOK);
    sm_config_set_jmp_pin (&c, pin);
    sm_config_set_fifo_join (&c, PIO_FIFO_JOIN_RX);

    return c;
}

static pio_sm_config wdog_config (uint offset, int pin)
{
    pio_sm_config c;
    c = watchdog_program_get_default_config (offset);
    // Input and jump pin: the pin to watch
    sm_config_set_in_pins (&c, pin);
    sm_config_set_jmp_pin (&c, pin);
    // side set: clock_ok signal
    sm_config_set_sideset_pins (&c, CLOCKOK);
    sm_config_set_out_shift (&c, true, false, 32);

    return c;
}

static void setup_pios (uint mflags, uint speed, uint txspeed)
{
    int i;
    uint offset;
    pio_sm_config rxconfig, txconfig, blinkconfig, imconfig, freqconfig;
    pio_sm_config wdogconfig;
    int wdog_period;
    int rxpin, txpin, clkpin, freqpin, wdogpin;

    DPRINTF ("Setting up PIO state machines, flags 0x%02x, speed %d\n",
            mflags, speed);
    pio_clear_instruction_memory (pio0);
    pio_clear_instruction_memory (pio1);

    // Set all the output pins we use to PIO.
    DPRINTF ("Assigning pins to PIO\n");
    pio_gpio_init (TXPIO, TXDATA);
    pio_gpio_init (TXPIO, CLKOUT);
    pio_gpio_init (TXPIO, IMTXDATA);
    pio_gpio_init (RXPIO, RXRECDATA);
    pio_gpio_init (RXPIO, IMSTROBE);
    pio_gpio_init (TXPIO, LCPIN);
    pio_gpio_init (TXPIO, LDPIN);
    pio_gpio_init (WDOGPIO, CLOCKOK);
    // LEDs
    pio_gpio_init (RXPIO, SYNLED);
    pio_gpio_init (BLINKPIO, RXLED);
    pio_gpio_init (BLINKPIO, TXLED);

    // Initialize the pulse stretchers
    DPRINTF ("Initializing RX LED pulse stretcher\n");
    offset = pio_add_program (BLINKPIO, &pulse_stretch_program);
    blinkconfig = pulse_stretch_config (offset);
    pio_sm_init (BLINKPIO, RXLEDSM, offset, &blinkconfig);
    pio_sm_set_consecutive_pindirs (BLINKPIO, RXLEDSM, RXLED, 1, true);
    pio_sm_set_enabled (BLINKPIO, RXLEDSM, true);
    DPRINTF ("RX LED pulse stretcher active\n");
    // set: LED pin
    sm_config_set_set_pins (&blinkconfig, TXLED, 1);
    pio_sm_init (BLINKPIO, TXLEDSM, offset, &blinkconfig);
    pio_sm_set_consecutive_pindirs (BLINKPIO, TXLEDSM, TXLED, 1, true);
    pio_sm_set_enabled (BLINKPIO, TXLEDSM, true);
    DPRINTF ("TX LED pulse stretcher active\n");

    if (mflags & (INT_MODEM | RS232_LOCAL_CLK))
    {
        // local clock or integral modem, derive watchdog period from
        // nominal receive data rate
        wdog_period = clock_get_hz (clk_sys) / speed;
    }
    else
    {
        // modem clock, assume slow bits
        wdog_period = clock_get_hz (clk_sys) / 1200;
    }
    
    // Set up the right I/O state machines for the chosen mode
    if (mflags & INT_MODEM)
    {
        // Integral modem.  We want inmodem_xmit, inmodem_rxbit,
        // local_clk_recv.  But if raw waveform mode is requested, use
        // inmodem_rawxmit instaed.
        DPRINTF ("Initializing integral modem\n");
        rxpin = IMRXDATA;
        txpin = IMTXDATA;
        if (mflags & INT_LOOPBACK)
        {
            DPRINTF ("Internal loopback mode\n");
            rxpin = txpin = LDPIN;
        }
        else
        {
            DPRINTF ("Setting integral modem enable\n");
            gpio_put (IMEN, true);
        }
        // Frequency measurement uses the data strobe signal generated
        // by the clock/data recovery state machine.
        freqpin = wdogpin = IMSTROBE;
        offset = pio_add_program (RXPIO, &local_clk_recv_program);
        // Second argument true means configure for integral modem case.
        rxconfig = local_clk_recv_config (offset, RXRECDATA);
        if (mflags & RAW_WAVEFORM)
        {
            offset = pio_add_program (TXPIO, &inmodem_rawxmit_program);
            txconfig = inmodem_rawxmit_config (offset, txspeed, txpin);
            DPRINTF ("Setting raw waveform mode\n");
            wdogpin = rxpin;
            wdog_period *= 2;
        }
        else
        {
            offset = pio_add_program (TXPIO, &inmodem_xmit_program);
            txconfig = inmodem_xmit_config (offset, txspeed, txpin);
        }
        offset = pio_add_program (IMBITPIO, &inmodem_rxbit_program);
        imconfig = inmodem_rxbit_config (offset, speed, rxpin);
        pio_sm_init (IMBITPIO, IMBITSM, offset, &imconfig);
        // Set pindirs.  Note that output mode must be set after input
        // mode, for pins that are used both ways, otherwise the pin
        // ends up not having output enabled.
        pio_sm_set_consecutive_pindirs (RXPIO, RXSM, RXRECDATA, 1, false);
        pio_sm_set_consecutive_pindirs (IMBITPIO, IMBITSM, rxpin, 1, false);
        pio_sm_set_consecutive_pindirs (IMBITPIO, IMBITSM, RXRECDATA, 2, true);
        pio_sm_set_consecutive_pindirs (TXPIO, TXSM, txpin, 1, true);
        // Start the bit stream recovery SM
        pio_sm_set_enabled (IMBITPIO, IMBITSM, true);
        DPRINTF ("Bit recovery SM active\n");
    }
    else if (mflags & RS232_LOCAL_CLK)
    {
        // RS-232 with clock generated locally (output).  We want
        // local_clk_xmit, local_clk_recv.  Internal loopback, when
        // integral modem is not selected, implies RS-232 local clock.
        DPRINTF ("Initializing RS-232, local clock\n");
        rxpin = RXDATA;
        txpin = TXDATA;
        clkpin = CLKOUT;
        if (mflags & INT_LOOPBACK)
        {
            DPRINTF ("Internal loopback mode\n");
            rxpin = txpin = LDPIN;
            clkpin = LCPIN;
        }
        else
        {
            DPRINTF ("Enabling RS-232\n");
            gpio_put (RS232EN, true);
        }
        freqpin = wdogpin = clkpin;
        offset = pio_add_program (RXPIO, &local_clk_recv_program);
        // Second argument true means configure for RS-232 case.
        rxconfig = local_clk_recv_config (offset, rxpin);
        offset = pio_add_program (TXPIO, &local_clk_xmit_program);
        txconfig = local_clk_xmit_config (offset, speed, clkpin, txpin);
        // Set pindirs.  Note that output mode must be set after input
        // mode, for pins that are used both ways, otherwise the pin
        // ends up not having output enabled.
        pio_sm_set_consecutive_pindirs (RXPIO, RXSM, rxpin, 1, false);
        pio_sm_set_consecutive_pindirs (TXPIO, TXSM, txpin, 1, true);
        pio_sm_set_consecutive_pindirs (TXPIO, TXSM, clkpin, 1, true);
    }
    else
    {
        // RS-232, modem supplied clock.  We want modem_clk_xmit,
        // modem_clk_recv.
        DPRINTF ("Initializing RS-232, modem supplied clock\n");
        DPRINTF ("Enabling RS-232\n");
        freqpin = wdogpin = RXCLKIN;
        gpio_put (RS232EN, true);
        offset = pio_add_program (RXPIO, &modem_clk_recv_program);
        rxconfig = modem_clk_recv_config (offset);
        offset = pio_add_program (TXPIO, &modem_clk_xmit_program);
        txconfig = modem_clk_xmit_config (offset);
        // Receive SM gets data and clock which is next after data
        pio_sm_set_consecutive_pindirs (RXPIO, RXSM, RXDATA, 2, false);
        pio_sm_set_consecutive_pindirs (TXPIO, TXSM, TXDATA, 1, true);
        pio_sm_set_consecutive_pindirs (TXPIO, TXSM, TXCLKIN, 1, false);
    }
    // Set up the frequency measurement state machine
    avg_period = 0;
    offset = pio_add_program (FREQPIO, &freq_program);
    freqconfig = freq_config (offset, freqpin);
    pio_sm_set_consecutive_pindirs (FREQPIO, FREQSM, CLOCKOK, 1, false);
    pio_sm_set_consecutive_pindirs (FREQPIO, FREQSM, freqpin, 1, false);
    pio_sm_init (FREQPIO, FREQSM, offset, &freqconfig);
    pio_sm_set_enabled (FREQPIO, FREQSM, true);

    // Set up the bit clock watchdog
    offset = pio_add_program (WDOGPIO, &watchdog_program);
    wdogconfig = wdog_config (offset, wdogpin);
    pio_sm_set_consecutive_pindirs (WDOGPIO, WDOGSM, wdogpin, 1, false);
    pio_sm_set_consecutive_pindirs (WDOGPIO, WDOGSM, CLOCKOK, 1, true);
    pio_sm_init (WDOGPIO, WDOGSM, offset, &wdogconfig);
    // Set up watchdog machine.
    //  pull
    //  mov	x, osr
    pio_sm_put (WDOGPIO, WDOGSM, wdog_period);
    pio_sm_exec (WDOGPIO, WDOGSM, pio_encode_pull (false, false));
    pio_sm_exec (WDOGPIO, WDOGSM, pio_encode_mov (pio_x, pio_osr));
    pio_sm_set_enabled (WDOGPIO, WDOGSM, true);
    

    // Build the "resynchronize" jump
    rxsm_jmptop = pio_encode_jmp (rxtop);
    // Initialize rx and tx state machines
    DPRINTF ("Starting RX state machine\n");
    pio_sm_init (RXPIO, RXSM, rxtop, &rxconfig);
    DPRINTF ("Starting TX state machine\n");
    pio_sm_init (TXPIO, TXSM, txtop, &txconfig);

    // All the receive cases have this output pin in common:
    pio_sm_set_consecutive_pindirs (RXPIO, RXSM, SYNLED, 1, true);
    DPRINTF  ("RX pindirs set\n");
    
    // All set up, now start rx and tx state machines.  We have to
    // execute some setup instructions before starting the machines.
    // Set up rx machine.
    //  pull
    //  mov	y, osr
	//  mov	isr, null
    DPRINTF ("Setting up RX sync\n");
    pio_sm_put (RXPIO, RXSM, SYN4);
    pio_sm_exec (RXPIO, RXSM, pio_encode_pull (false, false));
    pio_sm_exec (RXPIO, RXSM, pio_encode_mov (pio_y, pio_osr));
    pio_sm_exec (RXPIO, RXSM, pio_encode_mov (pio_isr, pio_null));
    // Set receive FIFO join; there is no pio_sm_xyz function to do
    // this at runtime for some reason, so do it the hard way.  We
    // couldn't do this at configuration time because we need the TX
    // FIFO for the setup just above, to push the SYN4 value into the
    // state machine.
    hw_set_bits(&RXPIO->sm[RXSM].shiftctrl, PIO_SM0_SHIFTCTRL_FJOIN_RX_LSB);
    pio_sm_set_enabled (RXPIO, RXSM, true);
    DPRINTF ("RX active\n");
    
    // Set up tx machine.
    //  pull
    //  mov	x, osr
    DPRINTF ("Setting up TX sync\n");
    pio_sm_put (TXPIO, TXSM, SYN4);
    pio_sm_exec (TXPIO, TXSM, pio_encode_pull (false, false));
    pio_sm_exec (TXPIO, TXSM, pio_encode_mov (pio_x, pio_osr));
    if (mflags & INT_MODEM)
    {
        // Integral modem case, stuff line startup sequence into the
        // FIFO (before starting the state machine so it will send
        // this first).  The spec says at least four ones followed by
        // at least two zeroes; we send 32 of each just to make sure.
        // The first word is supplied by an instruction to force it
        // into the output shift register.  That replaces the SYN4
        // that was there before which left the OSR full.  See
        // EK-DMCLU-TM-002 (DMC11 Synchronous Line Unit Maintenance
        // Manual) page 4-76 for details.
        pio_sm_exec (TXPIO, TXSM, pio_encode_mov_not (pio_osr, pio_null));
        pio_sm_put (TXPIO, TXSM, 0);
    }
    pio_sm_set_enabled (TXPIO, TXSM, true);
    DPRINTF ("TX active\n");
}

// Set up core 1.  Note this is only done once since CPU1 can't be
// stopped.
static void start_cpu1 (void)
{
    if (core1_active)
    {
        DPRINTF ("Core 1 already started\n");
        return;
    }
    core1_active = true;
    DPRINTF ("Starting core 1\n");

    // Start core 1 running the receive framer
    multicore_launch_core1 (ddcmp_cpu1);
}

// Stop the DDCMP framer
static void stop_ddcmp (void)
{
    int i, pin;

    // TODO: flush the queue (library function for that) But why is
    // that needed?
    
    // Stop all our state machines.  Note that IMBITSM may not be in
    // use, it's only used for the integral modem case, but stopping
    // it can't hurt.
    rx_enabled = false;
    dmc_syn_al = dmc = raw_wave = false;
    bist = false;
    pio_sm_set_enabled (RXPIO, RXSM, false);
    pio_sm_set_enabled (TXPIO, TXSM, false);
    pio_sm_set_enabled (IMBITPIO, IMBITSM, false);
    pio_sm_set_enabled (BLINKPIO, RXLEDSM, false);
    pio_sm_set_enabled (BLINKPIO, TXLEDSM, false);

    // Remove all programs
    pio_clear_instruction_memory (pio0);
    pio_clear_instruction_memory (pio1);

    // Make sure DMA is stopped
    dma_channel_abort (txdma);
    tbuf_fill = tbuf_empty = 0;
    
    // Set all pins to SIO, input.
#if DEBUG
#define FIRST 2
#else
#define FIRST 02
#endif
    // Initialize all pins and set them all to be pulled down, so
    // no-connect pins are in a well defined state.
    for (i = FIRST; i <= 29; i++)
    {
        gpio_init (i);
        gpio_pull_down (i);
    }
    // Set direction for output pins.  Note that gpio_init supplies an
    // output value of zero as part of its initialization.  Also turn
    // off the pulldown since we're now driving the pin.  Set the
    // default drive strength to 2 mA (minimum) and fast slew rate.
    for (i = 0; i < sizeof (opins); i++)
    {
        pin = opins[i];
        gpio_set_dir (pin, true);
        gpio_disable_pulls (pin);
        gpio_set_drive (pin, PADS_BANK0_GPIO0_DRIVE_VALUE_2MA);
        gpio_set_slew (pin, 1);
    }
    // Set LED output drive strength to 12 mA
    gpio_set_drive (SYNLED, PADS_BANK0_GPIO0_DRIVE_VALUE_12MA);
    gpio_set_drive (RXLED, PADS_BANK0_GPIO0_DRIVE_VALUE_12MA);
    gpio_set_drive (ACTLED, PADS_BANK0_GPIO0_DRIVE_VALUE_12MA);
    gpio_set_drive (TXLED, PADS_BANK0_GPIO0_DRIVE_VALUE_12MA);
    // Pull up the LOOPTEST pin, it is active-low.
    gpio_pull_up (LOOPTEST);
}

static void start_ddcmp (uint mflags, uint speed, uint txspeed)
{
    int i;

    // Fail if already active
    if (rx_enabled)
    {
        status_msg.last_cmd_sts = CMD_ACTIVE;
        return;
    }
    bist = (mflags & BIST) != 0;
    if (mflags & INT_LOOPBACK)
    {
        if ((mflags & (INT_MODEM | RS232_LOCAL_CLK)) == 0)
        {
            mflags |= RS232_LOCAL_CLK;
        }
    }
    // validate arguments
    if (mflags & INT_MODEM)
    {
        // Internal modem
        if (speed == 0 && (mflags & INT_LOOPBACK) != 0)
        {
            // For internal loopback, supply default speed of 1 Mbps
            speed = 1000000;
        }
    }
    if (!(mflags & SPLIT) || !(mflags & INT_MODEM))
    {
        txspeed = speed;
    }
    if ((mflags & (INT_MODEM | RS232_LOCAL_CLK)) != 0 &&
        ((speed > 10000000 || speed < 500) ||
         (txspeed > 10000000 || txspeed < 500)))
    {
        DPRINTF ("speed out of range, %d %d\n", speed, txspeed);
        status_msg.last_cmd_sts = CMD_BAD_SPEED;
        bist = false;
        return;
    }
    if (mflags & RAW_WAVEFORM)
    {
        if (bist || !(mflags & INT_MODEM))
        {
            // RAW_WAVEFORM applies to internal modem only, and conflicts
            // with BIST because that doesn't know how to generate raw
            // waveform test data.
            DPRINTF ("Invalid combination of RAW_WAVEFORM with other modes\n");
            status_msg.last_cmd_sts = CMD_BAD_MODE;
            bist = false;
            return;
        }
        raw_wave = true;
    }

    // Initialize receive buffers to be all free
    for (i = 0; i < RBUFS; i++)
    {
        rbuf_ring[i].stat = BUF_EMPTY;
    }
    rbuf_empty = rbuf_fill = 0;

    setup_pios (mflags, speed, txspeed);

    DPRINTF ("Configuring DMA\n");
    // Build the DMA config settings (DMA CTRL register value) for the
    // transmit buffer DMA: 8 bit transfers, no write increment, data
    // request is transmit state machine output FIFO.  (Conditional
    // compile: 32 bit transfers.)
    dma_channel_config tx = dma_channel_get_default_config (txdma);
#if TX8
    channel_config_set_transfer_data_size (&tx, DMA_SIZE_8);
#else
    channel_config_set_transfer_data_size (&tx, DMA_SIZE_32);
#endif
    channel_config_set_dreq (&tx, pio_get_dreq (TXPIO, TXSM, true));

    // Load the two fixed DMA values: control register, write address
    // register.
    dma_channel_set_config (txdma, &tx, false);
    dma_channel_set_write_addr (txdma, &(TXPIO->txf[TXSM]), false);

    // Initialize the transmit ring indices, count, and full flag
    tbuf_empty = tbuf_fill = 0;
    tx_full = false;
    rx_enabled = true;
    if (mflags & DDCMP3)
    {
        if (mflags & INT_MODEM)
        {
            dmc_syn_al = true;
        }
        dmc = true;
    }
    else
    {
        dmc_syn_al = dmc = false;
    }
    status_msg.on = ON_ACT;
    syn_status = false;
    status_msg.mflags = mflags;
    status_msg.speed = speed;
    status_msg.txspeed = txspeed;
    start_cpu1 ();
    status_msg.last_cmd_sts = CMD_OK;
    gpio_put (ACTLED, true);

    DPRINTF ("DDCMP active\n");
}

#if DEBUG > 1
static uint8_t toprint (uint8_t c)
{
	if ((c > 31 && c < 127))// || c > 159)
    {
        return (c);
    }
	return ('.');
}

static void dumpbuf (uint8_t *buf, int count)
{
	int	b;
	int off, block;

	for (off = 0; off < count; off += 16, buf += 16)
    {
        printf ("%04x/ ", off);
        for (b = 0; b < 16; b++)
        {
            printf ("%02x ", buf[b]);
            if (b == 7)
            {
                printf (" ");
            }
		}
		for (b = 0; b < 16; b++)
        {
            printf ("%c", toprint (buf[b]));
        }
		printf ("\n");
	}
}
#else // DEBUG
#define dumpbuf(buf, cnt)
#endif

void handle_rbuf (void)
{
    struct ddcmp_rxframe *df = &rbuf_ring[rbuf_empty];

    if (send_status)
    {
        if (tud_network_can_xmit (sizeof (status_msg)))
        {
            DPRINTF ("requesting status transmit\n");
            tud_network_xmit (&status_msg, MINE (sizeof (status_msg)));
            return;
        }
    }
    if (df->stat != BUF_EMPTY)
    {
        if (bist)
        {
            DDPRINTF ("received frame %d, status %d, len %d, seq %d\n",
                      rbuf_empty, df->stat, df->data_len,
                      df->data[3] + (df->data[4] << 8));
            df->stat = BUF_EMPTY;
            rbuf_empty = (rbuf_empty + 1) % RBUFS;
        }
        else
        {
            if (tud_network_can_xmit (MINE (df->data_len + ETH_HDR + 2)))
            {
                DDPRINTF ("requesting transmit for frame %d\n", rbuf_empty);
                tud_network_xmit (df->bufhdr, MINE (df->data_len + ETH_HDR + 2));
            }
        }
    }
}

static void submit_txbuf (void *data, int plen)
{
    struct ddcmp_txframe *df = &tbuf_ring[tbuf_fill];

    // Set word count
#if !TX8
    plen >>= 2;
#endif
    df->data_len = plen;
    df->data_start = data;

    // Now send the descriptor to CPU 1.  We don't need to check for
    // room in the FIFO since that has 8 entries, which is the max
    // number of transmit buffers.
    sio_hw->fifo_wr = (uint32_t) df;
    txblink ();

    DDPRINTF ("Submitted tx %d, len %d\n", tbuf_fill, plen);

    // Update transmit ring index
    tbuf_fill = (tbuf_fill + 1) % TBUFS;
}

static void command (const void *_cmd, uint16_t size)
{
    const struct command_msg_t *cmd = (const struct command_msg_t *) _cmd;
    int plen;
    struct ddcmp_txframe *df;
    
    // Always reply with status
    send_status = true;
    
    if (size < 2 ||
        (cmd->cmd == REQ_START &&
         (size < 8 || (cmd->mflags & SPLIT) && size < sizeof (*cmd))))
    {
        DPRINTF ("Command too short, cmd %d len %d\n", cmd->cmd, size);
        status_msg.last_cmd_sts = CMD_BAD_MSG;
        return;
    }
    DPRINTF ("Processing command %d\n", cmd->cmd);
    switch (cmd->cmd)
    {
    case REQ_STATUS:
        break;
    case REQ_START:
        start_ddcmp (cmd->mflags, cmd->speed, cmd->txspeed);
        break;
    case REQ_STOP:
        rx_enabled = false;
        stop_ddcmp ();
        status_msg.on = 0;
        status_msg.last_cmd_sts = CMD_OK;
        break;
    case REQ_RAWTX:
        plen = size - 2;
#if !TX8
        if (plen & 3)
        {
            DPRINTF ("Raw transmit not word aligned, %d\n", plen);
            status_msg.last_cmd_sts = CMD_BAD_MSG;
            return;
        }
#endif
        if (plen > RDATA)
        {
            DPRINTF ("Raw transmit too long, %d\n", plen);
            status_msg.last_cmd_sts = TX_BAD_LENGTH;
            return;
        }
        if (!rx_enabled)
        {
            DPRINTF ("Raw transmit when off\n");
            status_msg.last_cmd_sts = TX_OFF;
            return;
        }
        df = &tbuf_ring[tbuf_fill];
        memcpy (df->data, &cmd->mflags, plen);
        submit_txbuf (df->data, plen);
        send_status = false;
        break;
    default:
        status_msg.last_cmd_sts = CMD_UNKNOWN;
    }
}

static void crccpy (uint8_t *dst, const uint8_t *restrict src, int cnt)
{
    uint16_t crc = 0;
    int i;
    uint8_t b;

    for (i = 0; i < cnt; i++)
    {
        *dst++ = b = *src++;
        crc = update_crc (crc, b);
    }
    *dst++ = crc & 0xff;
    *dst++ = crc >> 8;
}

static bool transmit (uint8_t *pkt, uint16_t size)
{
    struct ddcmp_txframe *df = &tbuf_ring[tbuf_fill];
    int plen;
    
    if (size < 6)
    {
        DPRINTF ("bad transmit length %d\n", size);
        status_msg.last_cmd_sts = TX_BAD_LENGTH;
        send_status = true;
        return true;
    }
    if (!rx_enabled)
    {
        DPRINTF ("transmit when off\n");
        status_msg.last_cmd_sts = TX_OFF;
        send_status = true;
        return true;
    }
    if (raw_wave)
    {
        status_msg.last_cmd_sts = TX_WHEN_RAW;
        send_status = true;
        return true;
    }
    if (df->data_len != 0)
    {
        // Next buffer still busy, so all buffers are still being
        // transmitted.
        tx_full = true;
        return false;
    }
    tx_full = false;
    
    // Copy the header and set header CRC
    crccpy (df->data, pkt, 6);
    
    if (pkt[0] == ENQ)
    {
        // control frame
        plen = 8;
        if (dmc)
        {
            // In DDCMP V3 compatibility mode, force QSYN and SEL to
            // one.
            pkt[2] |= 0xc0;
        }
    }
    else
    {
        if (dmc)
        {
            // In DDCMP V3 compatibility mode, force QSYN and SEL to
            // zero.  Note this is critical; it appears that the DMC11
            // will mess up its memory address register if this rule
            // is violated.  See the DMC11 Microprocessor manual,
            // appendix I.
            pkt[2] &= 0x3f;
        }
        plen = pkt[1] | ((pkt[2] & 0x3f) << 8);
        if (plen == 0 || plen > MAXLEN || plen + 8 > size)
        {
            DPRINTF ("Bad DDCMP length %d buffer size %d\n", plen, size);
            status_msg.last_cmd_sts = TX_BAD_LENGTH;
            send_status = true;
            return true;
        }
        
        // Put the DDCMP payload after the header.  For maximal
        // transparency to existing SIMH code we use a frame laid out
        // as sent with the CRC field included, but the value there is
        // not actually used.  Instead, we generate the CRC here and
        // skip over what the host sent.  The ending CRC (header CRC
        // for control frames, data CRC for data frames) is ignored
        // and need not be included in the packet length.
        crccpy (df->data + 8, pkt + 8, plen);
        plen += 8 + 2;
    }
    // Update counters
    status_msg.txframes++;
    status_msg.txbytes += plen;

    // Set trailing DEL, plus SYN to pad to 4 byte multiple
    df->data[plen++] = DEL;
#if !TX8
    df->data[plen++] = SYN;
    df->data[plen++] = SYN;
    df->data[plen++] = SYN;
    plen &= ~3;
#endif
    MB;

    // Add leading SYN count, then start the DMA.  Note that we do not
    // have support for sending abutting messages (DDCMP V4).  It
    // would be nice to do so, although the performance benefit is
    // probably not all that large, but the rules for doing it are
    // amazingly complex -- see section 5.1.3.2 in the DDCMP V4.0
    // specification.  And while the rules for when a "short sync
    // sequence" can be sent (section 5.1.3.3) are not quite as bad,
    // they likewise don't seem to be worth the effort.  In other
    // words, we always send a "long sequence" (8 SYN bytes, except on
    // DMC11 integral modem case where we use 10).
    if (dmc_syn_al)
    {
        // DDCMP V3 on DMC11-AL compatibility
        submit_txbuf (df->syn, plen + SYN_CNT_V3);
    }
    else
    {
        submit_txbuf (df->syn + (SYN_CNT_V3 - SYN_CNT), plen + SYN_CNT);
    }
    return true;
}

// Handle frames from host
static bool handle_frame (const uint8_t *src, uint16_t size)
{
    int plen = size - 16;
    int plen2;

    if (src[12] != 0x60 || src[13] != 0x06)
    {
        DDPRINTF ("Ignoring other protocol %02x-%02x\n", src[12], src[13]);
        return true;
    }
    if (plen < 1)
    {
        DPRINTF ("Frame from host too short, %d\n", size);
        return true;
    }
    plen2 = src[14] + (src[15] << 8);
    if (plen2 > plen || plen2 < 1)
    {
        DPRINTF ("Length field is invalid, %d vs. %d\n", plen, plen2);
        return true;
    }
    plen = plen2;
    if (src[16] == DC1)
    {
        command (src + 16, plen);
        return true;
    }
    if (src[16] == SOH || src[16] == ENQ || src[16] == DLE)
    {
        // Data, send it.  But if BIST is active, silently discard all
        // transmit requests.
        if (bist)
        {
            return true;
        }
        return transmit ((uint8_t *) (src + 16), plen);
    }
    DPRINTF ("Unknown frame code %03o\n", src[16]);
    
    return true;
}

bool tud_network_recv_cb(const uint8_t *src, uint16_t size)
{
    bool ret = handle_frame (src, size);
    
    if (ret)
    {
        tud_network_recv_renew();
    }
    return ret;
}
        
// This handles a poll from USB to see if we have something to transmit.
uint16_t tud_network_xmit_cb (uint8_t *dst, void *ref, uint16_t arg)
{
    struct ddcmp_rxframe *df = &rbuf_ring[rbuf_empty];
    int len = 0;

    if (send_status)
    {
        send_status = false;
        // Figure out the "on" flags
        if (status_msg.on & ON_ACT)
        {
            if (gpio_get (SYNLED))
            {
                status_msg.on = ON_ACT | ON_SYN;
                syn_status = true;
            }
            else
            {
                status_msg.on = ON_ACT;
                syn_status = false;
            }
            if (gpio_get (CLOCKOK))
            {
                status_msg.on |= ON_CLKOK;
            }
        }
        
        // Convert average period to frequency.  For the RS-232 case,
        // the period is the modem clock period, so its reciprocal is
        // the frequency.  For the integral modem case we use the
        // "strobe" test signal, which has one pulse (one cycle, two
        // edges) per recovered data bit.
        //
        // The avg_period variable is the moving average of the raw
        // measurements from the frequency PIO state machine, which
        // counts every two CPU cycles and delivers the sum of 32
        // measurements.  But note that due to loop overhead, some
        // cycles are not accounted for, so they are added in here.
        int period = avg_period;
        float freq;
        if (period != 0)
        {
            // We keep the moving average in units of 1/32 so correct
            // for that.
            period >>= 5;
            freq = (((float) clock_get_hz (clk_sys)) / ((period + 32) * 2)) * 32;
        }
        else
        {
            freq = 0;
        }
        status_msg.freq = freq;

        len = MINE (sizeof (status_msg));
        memcpy (dst, &status_msg, len);
        DPRINTF ("Sending status report length %d\n", len);
    }
    else if (df->stat != BUF_EMPTY)
    {
        df->data_len += 2;
        len = df->data_len + ETH_HDR + 2;
        DDPRINTF ("Sending to host received frame %d status %d len %d\n",
                  rbuf_empty, df->stat, df->data_len);
        dumpbuf (df->bufhdr, len);

        // Give it to USB (just the real payload, padding is not
        // written to the USB buffer).
        memcpy (dst, df->bufhdr, len);
        // Report the padded (if requested) length
        len = MINE (len);
        // Mark buffer as handled
        df->stat = BUF_EMPTY;
        rbuf_empty = (rbuf_empty + 1) % RBUFS;
    }
    return len;
}

// Check for a transmit done that transmit is waiting for.  The
// transmit request handler sets a flag if there are no free transmit
// buffers when a transmit request arrives.  Here we check if that
// flag is set but the next transmit buffer in the ring is now free.
// If yes, tell the USB machinery that it can submit the rejected
// buffer again.
static void handle_tdone (void)
{
    if (tx_full && tbuf_ring[tbuf_fill].data_len == 0)
    {
        tx_full = false;
        if (!bist)
        {
            tud_network_recv_renew ();
        }
    }
}

void tud_network_init_cb(void)
{
    // Nothing to do
}

// Unused, here to satisfy a reference in net_device.c
void rndis_class_set_handler(uint8_t *data, int size)
{
}

// The BIST messages are DDCMP Maintenance packets with BIST_LEN bytes
// of payload.  There is a 4 byte sequence number at the start of the
// payload, the rest is an mixed data pattern.  The low order 2 bytes
// of the sequence number are also inserted into the header (in bytes
// normally zero in maintenance messages) so we have the sequence
// number available when reporting packets received with header CRC
// error.  The data length is a multiple of 4 plus 1, so that with CRC
// and DEL byte appended we're at a multiple of 4, ensuring no SYN
// padding at the end even if 32 bit transmit FIFO is used.
#define BIST_LEN 501
uint8_t bist_data[8 + BIST_LEN] = {
    DLE, BIST_LEN & 0xff, BIST_LEN >> 8
};
uint32_t bist_seq;

static void send_bist_data (void)
{
    if (!tx_full)
    {
        memcpy (bist_data + 8, &bist_seq, 4);
        memcpy (bist_data + 3, &bist_seq, 2);
        if (transmit (bist_data, sizeof (bist_data)))
        {
            bist_seq++;
            if ((bist_seq % 1000) == 0)
            {
                // Every 1000 frames, send status to the host
                send_status = true;
            }   
        }
    }
}

// One-time initialization
static void init_once (void)
{
    int i, j;
    uint8_t *p;
    
    DPRINTF ("Initializing status buffer\n");
    memcpy (&status_msg, default_hdr, sizeof (default_hdr));
    status_msg.data_len = sizeof (status_msg) - RBUF_OFF;
    status_msg.dc1 = DC1;
    status_msg.stat = DDCMP_OK;
    status_msg.on = 0;
    status_msg.sdusize = MAXLEN;
    strcpy (status_msg.version, VERSION);
    
    // No need to clear the counters since this buffer is static so
    // it's zeroed at program load.

    // Fill in the receive buffer Ethernet headers
    for (i = 0; i < RBUFS; i++)
    {
        // Set up the Ethernet header
        memcpy (rbuf_ring[i].bufhdr, default_hdr, sizeof (default_hdr));
    }

    // Fill in the transmit SYN bytes
    for (i = 0; i < TBUFS; i++)
    {
        // Mark it as free
        tbuf_ring[i].data_len = 0;
        
        // Set up the sync bytes
        for (j = 0; j < sizeof (tbuf_ring[0].syn); j++)
        {
            tbuf_ring[i].syn[j] = SYN;
        }
    }
    
    // Get a DMA channel
    txdma = dma_claim_unused_channel (true);
    txdmask = 1 << txdma;
    txcsr = dma_channel_hw_addr (txdma);

    // Now initialize everything else
    stop_ddcmp ();

    // The test data pattern starts after the 4-byte sequence number
    // field (which is written when the buffer is queued for
    // transmit).
    p = bist_data + 12;
    while (p < bist_data + sizeof (bist_data))
    {
        *p++ = 0xaa;
        *p++ = 0xcc;
        *p++ = 0xff;
        *p++ = 0x00;
    }
    
    DPRINTF ("One-time init done\n");
}

int main ()
{
    union 
    {
        pico_unique_board_id_t uid;
        uint32_t id32[2];
    } id;
    uint32_t id24;
    int i;
    
    // Start by changing the system clock to 120 MHz, which is an
    // integral multiple of the fast line speeds we want to support.
    // That way we can support the exact nominal values rather than
    // having to choose between jitter (due to fractional dividers) or
    // rates that are not actually what was asked for.
    // Various code snippets here were lifted from the Pico SDK.

    // 1. Set the sys clock to use the ref clock
    // CLK SYS = CLK_REF.
    uint clk_ref_freq = clock_get_hz (clk_ref);
    clock_configure (clk_sys,
                     CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF,
                     0,
                     clk_ref_freq,
                     clk_ref_freq);

    // 2. Set the sys PLL to 120 MHz.  This waits for PLL lock.
    pll_init (pll_sys, 1, 1440 * MHZ, 6, 2);

    // 3. Set sys clock back to use the PLL
    // CLK SYS = PLL SYS (120 MHz) / 1 = 120 MHz
    clock_configure (clk_sys,
                     CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                     CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
                     120 * MHZ,
                     120 * MHZ);

    // 4. Peri clock uses sys clock.  Just call clock_configure to set
    // it up, though it's normally already set that way and all that's
    // changing is the declared frequency.
    clock_configure (clk_peri,
                     0,
                     CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                     120 * MHZ,
                     120 * MHZ);

    // Get unique ID, use it to make up Ethernet addresses.  The
    // interface address is forced to be even, the framer address is
    // one higher.
    pico_get_unique_board_id (&id.uid);
    id24 = (id.id32[0] ^ id.id32[1]) & 0xfffffe;
    default_hdr[3] = (id24 >> 16) & 0xff;
    default_hdr[4] = (id24 >>  8) & 0xff;
    default_hdr[5] = (id24 >>  0) & 0xff;
    id24++;
    default_hdr[6 + 3] = (id24 >> 16) & 0xff;
    default_hdr[6 + 4] = (id24 >>  8) & 0xff;
    default_hdr[6 + 5] = (id24 >>  0) & 0xff;
    memcpy (tud_network_mac_address, default_hdr, 6);
    
#if DEBUG
    // Initialize stdio and the UART.  Note this has to be done after
    // the manipulation of the system clock speeds.
    stdio_init_all ();
#endif
    DPRINTF ("\n\nInitializing DDCMP framer\n");
    DPRINTF (VERSION "\n");
    DPRINTF ("Max packet length: %d\n", MAXLEN);
    DPRINTF ("Interface address: ");
    for (i = 0; i < 5; i++)
        DPRINTF ("%02X-", default_hdr[i]);
    DPRINTF ("%02X\n", default_hdr[5]);
    
    init_once ();

    // See if selftest is requested (TEST pin pulled down)
    if (gpio_get (LOOPTEST) == 0)
    {
        DPRINTF ("Selftest requested\n");
        DPRINTF ("Starting integral modem BIST, internal loopback, 1 Mbps\n");
        // We use the integral modem (since that's the most complex
        // case) at max standard speed (1 Mb/s).
        start_ddcmp (BIST | INT_LOOPBACK | INT_MODEM, 1000000, 1000000);
    }
    
    /* initialize TinyUSB */
    DPRINTF ("Initializing USB\n");
    tusb_init();
    send_status = true;

    DPRINTF ("Entering main loop\n");
    while (1) 
    {
        if (bist)
        {
            send_bist_data ();
        }
        if (status_msg.on & ON_ACT)
        {
            // Framer is on, see if we need to update host with new
            // SYN status.
            if (gpio_get (SYNLED))
            {
                if (!syn_status)
                {
                    send_status = true;
                    DPRINTF ("Status request, SYN now true\n");
                }
            }
            else if (syn_status)
            {
                send_status = true;
                DPRINTF ("Status request, SYN now false\n");
            }
        }
        tud_task();
        handle_rbuf ();
        handle_tdone ();
    }
    // UNREACHABLE:
    return 0;
}
