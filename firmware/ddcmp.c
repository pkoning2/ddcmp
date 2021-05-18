//
#define DEBUG 1

#define __packed 
#define __aligned(x)

#include <string.h>

#include "bsp/board.h"
#include "tusb.h"

#include "ddcmp.pio.h"
#include <hardware/pio_instructions.h>
#include <hardware/timer.h>
#include <hardware/irq.h>
#include <hardware/dma.h>
#include <pico/multicore.h>
#include <hardware/clocks.h>

#ifdef DEBUG
// Debugging, if enabled, uses the UART at the default pins (pin 1 and
// 2, GPIO 0 and GPIO 1).
#include <stdio.h>
#include <pico/stdio_uart.h>

#define DPRINTF printf
#else
#define DPRINTF(fmt...) // empty
#endif
#define DDPRINTF(fmt...) // empty

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
#define DIFFPIO pio1
#define DIFFSM 0
// The two LED pulse stretcher SMs share the same program
#define BLINKPIO pio1
#define RXLEDSM  3
#define TXLEDSM  2

// Load addresses for these
uint16_t rxtop, txtop, imbittop, blinktop;

// Jumps back to top
uint16_t rxsm_jmptop;
uint16_t blinksm_jmptop;

// Core 1 related state
volatile bool core1_active = false;

// Receive (and more generally, the device) enabled state.
volatile bool rx_enabled = false;

// Selftest mode active
bool bist = true;//false;

// Pin numbers (GPIOn numbers, to be precise)
// Clock/data pins
#define TXCLKIN 12      // Transmit clock input from modem
#define TXDATA  15      // Transmit data output
#define RXDATA  13      // RS232 receive data input
#define RXCLKIN 14      // Receive clock input from modem, must be RXDATA+1
#define CLKOUT  11      // RS232 clock output (RX/TX both) to DTE
#define IMTXDATA 19     // Integral modem transmit data output
#define IMRXDATA 17     // Integral modem receive data input
// Enable pins
#define RS232EN 10      // RS-232 output enable
#define RSCLKEN 9       // RS-232 clock output enable
#define IMEN    18      // Integral Modem enable
// Slew control
#define IMSRL   16      // Integral modem transmit slew control (three state)
// Loopback test control
#define LOOPTEST 4      // Pull low to force power on selftest
// LED pin assignments
#define SYNLED   20     // In-sync indicator LED output
#define RXLED    21     // Packet received LED
#define ACTLED   25     // Device is active (RPico built-in LED)
#define TXLED    22     // Packet sent LED
// No-connect pins.  These are used when a signal needs to get from
// one state machine to another.  That applies to integral modem
// receive data (from demodulator), as well as to the loopback case.
// These pins are not connected to anything on the PC board, but they
// are enabled (both input and output circuits) so they have to be
// reserved for the purpose and configured as needed just like other
// pins.
#define RXRECDATA 8     // Integral modem recovered receive bit stream
#define LCPIN    6      // Loopback clock pin
#define LDPIN    7      // Loopback data pin

// These are pins we use as output; they are explicitly driven to 0 at
// initialization and ddcmp stop.
const uint8_t opins[] = {
    TXDATA,
    CLKOUT,
    IMTXDATA,
    RS232EN,
    RSCLKEN,
    SYNLED,
    RXLED,
    ACTLED,
    TXLED,
    LCPIN,
    LDPIN
};

// DDCMP character codes
#define SOH 0201        // SOH - start of data message
#define ENQ 0005        // ENQ - start of control message
#define DLE 0220        // DLE - start of maintenance message
#define SYN 0226        // SYN - synchronization code
#define DEL 0377        // DEL - pad after message trailer

#define DC1 0021        // DC1 - used here for command/status, not DDCMP

#define SYN4 0x96969696U
#define RXLIMIT (1500 - 2) // Max received total frame size to host
#define RDMAX (RXLIMIT - 8 - 2) // Max receive DDCMP data length
#define TXLIMIT 1500    // Max transmit total frame size from host
#define TDMAX (TXLIMIT - 6) // Max transmit DDCMP data length
#define MAXLEN (TDMAX < TDMAX ? RDMAX : TDMAX) // Max DDCMP payload
#define RDATA (MAXLEN + 8 + 2) // Frame space in rings
#define BUF_OFF 16      // Buffer offset to start of DDCMP frame
#define SYN_CNT 8       // Count of SYN bytes before tx frame start
#define RBUFS 8         // Number of buffers in receive ring
#define TBUFS 8         // Number of buffers in transmit ring

// Operating mode flags
#define DCE_MODE        1
#define INT_MODEM       2
#define RS232_LOCAL_CLK 4
#define INT_LOOPBACK    8

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
    BUF_EMPTY           // Buffer waiting to be filled
};

// DDCMP frame buffer layout (framer to host)
struct ddcmp_rxframe
{
    int data_len;
    uint8_t bufhdr[BUF_OFF - 2]; // room for "Ethernet" header
    uint16_t stat;          // Status (ddcmp_status code)
    uint8_t data[RDATA];    // DDCMP frame data (header + payload)
};

// Receive ring
struct ddcmp_rxframe rbuf_ring[RBUFS];
volatile int rbuf_empty = 0;
volatile int rbuf_fill = 0;

// DDCMP frame buffer layout (host to framer, transmit ring entry)
struct ddcmp_txframe
{
    uint32_t data_len;      // Total length (including SYN and DEL)
    uint8_t syn[SYN_CNT];   // SYN bytes before frame start
    uint8_t data[RDATA];    // DDCMP frame data (header + payload)
    uint8_t del;            // DEL after the CRC
};

// Transmit ring
struct ddcmp_txframe tbuf_ring[TBUFS];
volatile int tbuf_empty = 0;
volatile int tbuf_fill = 0;
volatile int tbuf_count = 0;

// Set to true in the transmit handler if it has no room for another
// transmit, checked in the main loop to re-request the transmit
// buffer from USB if the count now permits transmitting more.
volatile bool tx_full = false;

// DMA channel number
int txdma;
// DMA channel CSR pointer
volatile dma_channel_hw_t *txcsr;

// Default buffer header (Ethernet header)
static const uint8_t default_hdr[BUF_OFF] = {
    0xaa, 0x00, 0x03, 0x04, 0x05, 0x06,         // Dest:   aa-00-03-04-05-06
    0xaa, 0x00, 0x03, 0x04, 0x05, 0x07,         // Source: aa-00-03-04-05-07
    0x60, 0x06,                                 // Prot: 60-06 (customer)
    BUF_EMPTY, 0x00                             // Status
};

const uint8_t tud_network_mac_address[6] = {
    0xaa, 0x00, 0x03, 0x04, 0x05, 0x06         // Source: aa-00-03-04-05-06
};
const uint8_t framer_mac_address[6] = {
    0xaa, 0x00, 0x03, 0x04, 0x05, 0x07         // Source: aa-00-03-04-05-07
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
};

enum ddcmp_action
{
    REQ_STATUS,         // Get device status
    REQ_START,          // Start DDCMP engine
    REQ_STOP            // Stop DDCMP engine
};

// Status message.  This starts like a data message, except that the
// header byte is DC1 (0x11) rather than one of the three DDCMP start
// of header codes.
struct status_msg_t
{
    uint8_t bufhdr[BUF_OFF - 2];    // room for "Ethernet" header
    uint16_t stat;                  // Status (ddcmp_status code)
    uint8_t dc1;
    uint8_t on;
    uint16_t mflags;
    uint32_t speed;
    uint32_t rxframes;
    uint32_t rxbytes;
    uint32_t txframes;
    uint32_t txbytes;
    uint32_t hcrc_err;
    uint32_t crc_err;
    uint32_t len_err;
    uint32_t nobuf_err;
    uint32_t last_cmd_sts;          // Response code from last command
};

enum last_cmd_status_t
{
    CMD_OK = 0,             // Command was accepted
    CMD_BAD_MSG,            // Command message too short
    CMD_UNKNOWN,            // Unknown command code
    CMD_ACTIVE,             // Start command when already active
    CMD_BAD_SPEED,          // Speed is out of range
    TX_BAD_LENGTH,          // Transmit frame bad length
    TX_OFF,                 // Transmit request when device is off
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
    DPRINTF ("buffer %d (%d) done, status %d\n", rbuf_fill, rbuf_empty, stat);
    rbuf_ring[rbuf_fill].stat = stat;
    rbuf_fill = (rbuf_fill + 1) % RBUFS;
    rxblink ();
}

// Packet receive loop.  This runs in CPU 1, pulling received data
// words (4 bytes per entry) from the PIO receive FIFO and assembling
// DDCMP frames for the host.  The frames are delivered to the receive
// frame buffer ring.
void ddcmp_cpu1 (void)
{
    int i, bytes;
    bool ctrl;
    uint32_t rword;
    enum ddcmp_state ds = PKT_START;
    uint16_t crc;
    struct ddcmp_rxframe *frame;
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
                    break;
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
                DDPRINTF ("state %d byte 0x%02x\n", ds, b);
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
                            if (bytes > MAXLEN)
                            {
                                buf_done (DDCMP_LONG);
                                status_msg.len_err++;
                                syn_search ();
                                break;
                            }
                            frame->data_len += bytes;
                        }
                    }
                    else
                    {
                        // Bad CRC, report it and resync
                        if (ds == PAYLOAD)
                        {
                            status_msg.crc_err++;
                            buf_done (DDCMP_CRC);
                        }
                        else
                        {
                            status_msg.hcrc_err++;
                            buf_done (DDCMP_HCRC);
                        }
                        syn_search ();
                        ds = FLUSH;
                    }
                }
                break;
            case FLUSH:
                break;
            }
        }
    }
}

float clkdiv (int freq)
{
    return clock_get_hz (clk_sys) / ((float) freq);
}

// Set pio_sm_config clkdiv parameters given desired clock frequency.
// Lifted, with slight mods, from rp2_pio.c in micropython.
void sm_config_set_clkdiv_freq (pio_sm_config *c, int freq)
{
    // Frequency given in Hz, compute clkdiv from it.
    float div = clkdiv (freq);
    
    sm_config_set_clkdiv(c, div);
    DPRINTF ("Clock div %f\n", div);
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
    // FIFO: threshold 8, shift right, no autopull, joined
    sm_config_set_out_shift (&c, true, false, 8);
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
    // FIFO: threshold 8, shift right, no autopull, joined
    sm_config_set_out_shift (&c, true, false, 8);
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
    // FIFO: threshold 8 (for now), shift right, no autopull, joined
    sm_config_set_out_shift (&c, true, false, 8);
    sm_config_set_fifo_join (&c, PIO_FIFO_JOIN_TX);
    // output: integral modem tx data
    // side set: ditto
    sm_config_set_out_pins (&c, txpin, 1);
    sm_config_set_sideset_pins (&c, txpin);
    
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
    sm_config_set_clkdiv_freq (&c, speed * 16);
    // input: integral modem rx data; also jump pin
    sm_config_set_in_pins (&c, rxpin);
    sm_config_set_jmp_pin (&c, rxpin);
    // output: integral modem recovered data
    sm_config_set_out_pins (&c, RXRECDATA, 1);

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

static void dma_regs (void)
{
    DPRINTF ("txcsr (%p): %08x %08x %08x %08x\n", txcsr,
             txcsr->al1_ctrl, txcsr->read_addr,
             txcsr->write_addr, txcsr->transfer_count);
}

static void setup_pios (uint mflags, uint speed)
{
    int i;
    uint offset;
    pio_sm_config rxconfig, txconfig, blinkconfig, imconfig;
    int rxpin, txpin, clkpin;
    
    DPRINTF ("Setting up PIO state machines, flags %d, speed %d\n",
            mflags, speed);
    pio_clear_instruction_memory (pio0);
    pio_clear_instruction_memory (pio1);

    // Set all the output pins we use to PIO.
    DPRINTF ("Assigning pins to PIO\n");
    pio_gpio_init (TXPIO, TXDATA);
    pio_gpio_init (TXPIO, CLKOUT);
    pio_gpio_init (TXPIO, IMTXDATA);
    pio_gpio_init (RXPIO, RXRECDATA);
    pio_gpio_init (TXPIO, LCPIN);
    pio_gpio_init (TXPIO, LDPIN);
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

    // Set up the right I/O state machines for the chosen mode
    if (mflags & INT_MODEM)
    {
        // Integral modem.  We want inmodem_xmit, inmodem_rxbit,
        // local_clk_recv.
        DPRINTF ("Initializing integral modem\n");
        rxpin = IMRXDATA;
        txpin = IMTXDATA;
        if (mflags & INT_LOOPBACK)
        {
            DPRINTF ("Internal loopback mode\n");
            rxpin = txpin = LDPIN;
        }
        offset = pio_add_program (RXPIO, &local_clk_recv_program);
        // Second argument true means configure for integral modem case.
        rxconfig = local_clk_recv_config (offset, RXRECDATA);
        DPRINTF ("Receive SM configured\n");
        offset = pio_add_program (TXPIO, &inmodem_xmit_program);
        txconfig = inmodem_xmit_config (offset, speed, txpin);
        DPRINTF ("Transmit SM configured\n");
        offset = pio_add_program (IMBITPIO, &inmodem_rxbit_program);
        imconfig = inmodem_rxbit_config (offset, speed, rxpin);
        pio_sm_init (IMBITPIO, IMBITSM, offset, &imconfig);
        DPRINTF ("Receive clock/data recovery SM configured\n");
        // Set pindirs
        pio_sm_set_consecutive_pindirs (RXPIO, RXSM, RXRECDATA, 1, false);
        pio_sm_set_consecutive_pindirs (TXPIO, TXSM, txpin, 1, true);
        pio_sm_set_consecutive_pindirs (IMBITPIO, IMBITSM, rxpin, 1, false);
        pio_sm_set_consecutive_pindirs (IMBITPIO, IMBITSM, RXRECDATA, 1, true);
        DPRINTF ("Pindirs set\n");
        // Start the bit stream recovery SM
        pio_sm_set_enabled (IMBITPIO, IMBITSM, true);
        DPRINTF ("Bit recovery SM active\n");
    }
    else if (mflags & (RS232_LOCAL_CLK | INT_LOOPBACK))
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
        offset = pio_add_program (RXPIO, &local_clk_recv_program);
        // Second argument true means configure for RS-232 case.
        rxconfig = local_clk_recv_config (offset, rxpin);
        offset = pio_add_program (TXPIO, &local_clk_xmit_program);
        txconfig = local_clk_xmit_config (offset, speed, clkpin, txpin);
        // Set pindirs
        pio_sm_set_consecutive_pindirs (RXPIO, RXSM, rxpin, 1, false);
        pio_sm_set_consecutive_pindirs (TXPIO, TXSM, txpin, 1, true);
        pio_sm_set_consecutive_pindirs (TXPIO, TXSM, clkpin, 1, true);
    }
    else
    {
        // RS-232, modem supplied clock.  We want modem_clk_xmit,
        // modem_clk_recv.
        DPRINTF ("Initializing RS-232, modem supplied clock\n");
        offset = pio_add_program (RXPIO, &modem_clk_recv_program);
        rxconfig = modem_clk_recv_config (offset);
        offset = pio_add_program (TXPIO, &modem_clk_xmit_program);
        txconfig = modem_clk_xmit_config (offset);
        // Receive SM gets data and clock which is next after data
        pio_sm_set_consecutive_pindirs (RXPIO, RXSM, RXDATA, 2, false);
        pio_sm_set_consecutive_pindirs (TXPIO, TXSM, TXDATA, 1, true);
        pio_sm_set_consecutive_pindirs (TXPIO, TXSM, TXCLKIN, 1, false);
    }
    // Build the "resynchronize" jump
    rxsm_jmptop = pio_encode_jmp (rxtop);
    // Initialize rx and tx state machines
    DPRINTF ("Starting RX state machine\n");
    pio_sm_init (RXPIO, RXSM, rxtop, &rxconfig);
    DPRINTF ("Starting TX state machine\n");
    pio_sm_init (TXPIO, TXSM, txtop, &txconfig);
    DPRINTF ("TX state machine active\n");

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
    pio_sm_set_enabled (TXPIO, TXSM, true);

    DPRINTF ("Configuring DMA\n");
    // Initialize receive buffers to be all free
    for (i = 0; i < RBUFS; i++)
    {
        rbuf_ring[i].stat = BUF_EMPTY;
    }
    rbuf_empty = rbuf_fill = 0;

    // Build the DMA config settings (DMA CTRL register value) for the
    // transmit buffer DMA: 8 bit transfers, no write increment, data
    // request is transmit state machine output FIFO.
    dma_channel_config tx = dma_channel_get_default_config (txdma);
    channel_config_set_transfer_data_size (&tx, DMA_SIZE_8);
    channel_config_set_dreq (&tx, pio_get_dreq (TXPIO, TXSM, true));

    // Load the two fixed DMA values: control register, write address
    // register.
    dma_channel_set_config (txdma, &tx, false);
    dma_channel_set_write_addr (txdma, &(TXPIO->txf[TXSM]), false);

    // Enable IRQ0
    dma_channel_set_irq0_enabled (txdma, true);
    
    // Initialize the transmit ring indices, count, and full flag
    tbuf_empty = tbuf_fill = 0;
    tbuf_count = 0;
    tx_full = false;
    
    DPRINTF ("DDCMP active\n");
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

// For some reason this isn't a standard function.
static void gpio_set_drive (int gpio, int strength)
{
    // Set drive strength bits
    hw_write_masked(&padsbank0_hw->io[gpio],
                    strength << PADS_BANK0_GPIO0_DRIVE_LSB,
                    PADS_BANK0_GPIO0_DRIVE_BITS);
}


// Stop the DDCMP framer
static void stop_ddcmp (void)
{
    int i, pin;
    
    // Stop all our state machines.  Note that IMBITSM may not be in
    // use, it's only used for the integral modem case, but stopping
    // it can't hurt.
    rx_enabled = false;
    pio_sm_set_enabled (RXPIO, RXSM, false);
    pio_sm_set_enabled (TXPIO, TXSM, false);
    pio_sm_set_enabled (IMBITPIO, IMBITSM, false);
    pio_sm_set_enabled (BLINKPIO, RXLEDSM, false);
    pio_sm_set_enabled (BLINKPIO, TXLEDSM, false);

    // Remove all programs
    pio_clear_instruction_memory (pio0);
    pio_clear_instruction_memory (pio1);

    // Make sure DMA is stopped
    irq_set_enabled (DMA_IRQ_0, false);
    dma_channel_abort (txdma);
    tbuf_fill = tbuf_empty = tbuf_count = 0;
    // Clear any pending DMA interrupt.  Leave the interrupt disabled;
    // it will be enabled by transmit requests.
    dma_hw->ints0 = 1 << DMA_IRQ_0;
    
    // Set all pins to SIO, input.
#ifdef DEBUG
#define FIRST 2
#else
#define FIRST 0
#endif
    for (i = FIRST; i <= 29; i++)
    {
        gpio_init (i);
    }
    // Set direction for output pins.  Note that gpio_init supplies an
    // output value of zero as part of its initialization.
    for (i = 0; i < sizeof (opins); i++)
    {
        pin = opins[i];
        gpio_set_dir (pin, true);
    }
    // Set LED output drive strength to 12 mA
    gpio_set_drive (SYNLED, PADS_BANK0_GPIO0_DRIVE_VALUE_12MA);
    gpio_set_drive (RXLED, PADS_BANK0_GPIO0_DRIVE_VALUE_12MA);
    gpio_set_drive (ACTLED, PADS_BANK0_GPIO0_DRIVE_VALUE_12MA);
    gpio_set_drive (TXLED, PADS_BANK0_GPIO0_DRIVE_VALUE_12MA);
}

#ifdef DEBUG
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

    if (send_status && !bist)
    {
        if (tud_network_can_xmit ())
        {
            DPRINTF ("requesting status transmit\n");
            tud_network_xmit (&status_msg, sizeof (status_msg));
            return;
        }
    }
    if (df->stat != BUF_EMPTY)
    {
        if (bist)
        {
            DPRINTF ("received frame %d, status %d, len %d, seq %d\n",
                     rbuf_empty, df->stat, df->data_len,
                     df->data[3] + (df->data[4] << 8));
            df->stat = BUF_EMPTY;
        }
        else
        {
            if (tud_network_can_xmit ())
            {
                DPRINTF ("requesting transmit for frame %d\n", rbuf_empty);
                tud_network_xmit (df->bufhdr, df->data_len + BUF_OFF);
            }
        }
        rbuf_empty = (rbuf_empty + 1) % RBUFS;
    }
}

static void command (const void *_cmd, uint16_t size)
{
    const struct command_msg_t *cmd = (const struct command_msg_t *) _cmd;
    int maxmul = 4;
    int speed;
    
    // Always reply with status
    send_status = true;
    
    if (size < 2 || (cmd->cmd == REQ_START && size < sizeof (*cmd)))
    {
        DPRINTF ("Command too short, cmd %d len %d\n", cmd->cmd, size);
        status_msg.last_cmd_sts = CMD_BAD_MSG;
        return;
    }
    switch (cmd->cmd)
    {
    case REQ_STATUS:
        break;
    case REQ_START:
        // Fail if already active
        if (rx_enabled)
        {
            status_msg.last_cmd_sts = CMD_ACTIVE;
            return;
        }
        // validate arguments
        speed = cmd->speed;
        if (cmd->mflags & INT_MODEM)
        {
            // Internal modem
            maxmul = 16;
            if (speed == 0 && (cmd->mflags & INT_LOOPBACK) != 0)
            {
                // For internal loopback, supply default speed of 1 Mbps
                speed = 1000000;
            }
        }
        if ((cmd->mflags & (INT_MODEM | RS232_LOCAL_CLK | INT_LOOPBACK)) != 0 &&
            (clkdiv (speed * 4) > 65535 ||
             clkdiv (speed * maxmul) < 1))
        {
            DPRINTF ("speed out of range, %d\n", speed);
            status_msg.last_cmd_sts = CMD_BAD_SPEED;
            return;
        }
        setup_pios (cmd->mflags, speed);
        rx_enabled = true;
        status_msg.on = true;
        start_cpu1 ();
        status_msg.last_cmd_sts = CMD_OK;
        break;
    case REQ_STOP:
        rx_enabled = false;
        stop_ddcmp ();
        status_msg.on = false;
        status_msg.last_cmd_sts = CMD_OK;
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

// Interrupt handler for DMA IRQ0
void dma_done (void)
{
    int te;
    struct ddcmp_txframe *df;

    // Clear the interrupt request
    dma_hw->ints0 = 1 << DMA_IRQ_0;
    
    // Advance the empty pointer
    tbuf_empty = te = (tbuf_empty + 1) % TBUFS;
    
    DPRINTF ("DMA done, count was %d\n", tbuf_count);
    
    // Decrement pending transmit count
    if (--tbuf_count != 0)
    {
        // Still non-zero, start up another DMA
        DPRINTF ("starting another DMA for %d\n", te);
        df = &tbuf_ring[te];
        txcsr->al1_read_addr = (uint32_t) (df->data);
        txcsr->al1_transfer_count_trig = df->data_len;
    }
}

static bool transmit (const uint8_t *pkt, uint16_t size)
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
    if (tbuf_count == TBUFS)
    {
        // All buffers are still being transmitted
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
    }
    else
    {
        plen = df->data[1] | ((df->data[2] & 0x3f) << 8);
        if (plen == 0 || plen > MAXLEN || plen > size + 6)
        {
            DPRINTF ("Bad DDCMP length %d buffer size %d\n", plen, size);
            status_msg.last_cmd_sts = TX_BAD_LENGTH;
            send_status = true;
            return true;
        }
        // Put the DDCMP payload after the header.  Note there is a
        // CRC in the transmit ring buffer but not in the buffer we
        // got from the host, hence the two different offsets.
        crccpy (df->data + 8, pkt + 6, plen);
        plen += 8 + 2;
    }
    // Set trailing DEL
    df->data[plen] = DEL;
    // Update counters
    status_msg.txframes++;
    status_msg.txbytes += plen;
    // Compute total length, including SYNs and DEL
    plen += SYN_CNT + 1;
    df->data_len = plen;
    // Mask the DMA done interrupt, then increment the pending count
    // and start DMA if not currently running.
    irq_set_enabled (DMA_IRQ_0, false);
    if (tbuf_count++ == 0)
    {
        DPRINTF ("Starting DMA\n");
        txcsr->al1_read_addr = (uint32_t) (df->data);
        txcsr->al1_transfer_count_trig = plen;
    }
    else
    {
        DPRINTF ("DMA already active\n");
    }
    DPRINTF ("transmit %d, count %d, len %d\n", tbuf_fill, tbuf_count, plen);
    irq_set_enabled (DMA_IRQ_0, true);
    txblink ();

    // Update transmit ring index
    tbuf_fill = (tbuf_fill + 1) % TBUFS;
    
    return true;
}

// Handle frames from host
bool tud_network_recv_cb(const uint8_t *src, uint16_t size)
{
    int plen = size - 14;
    
    DPRINTF ("recv_cb %p len %\n", src, size);

    if (src[12] != 0x60 || src[13] != 0x06)
    {
        DPRINTF ("Ignoring other protocol %02x-%02x\n", src[12], src[13]);
        return true;
    }
    if (plen < 1)
    {
        DPRINTF ("Frame from host too short, %d\n", size);
        return true;
    }
    if (src[14] == DC1)
    {
        command (src + 14, plen);
        return true;
    }
    if (src[14] == SOH || src[14] == ENQ || src[14] == DLE)
    {
        // Data, send it
        return transmit (src + 14, plen);
    }
    DPRINTF ("Unknown frame code %03o\n", src[14]);
    
    return true;
}

// This handles a poll from USB to see if we have something to transmit.
uint16_t tud_network_xmit_cb (uint8_t *dst, void *ref, uint16_t arg)
{
    struct ddcmp_rxframe *df = &rbuf_ring[rbuf_empty];
    int len = 0;

    if (send_status)
    {
        send_status = false;
        len = sizeof (status_msg);
        memcpy (dst, &status_msg, len);
        DPRINTF ("Sending status report length %d\n", len);
    }
    else if (df->stat != BUF_EMPTY)
    {
        len = df->data_len + BUF_OFF;
        DPRINTF ("Sending to host received frame %d status %d len %d\n",
                 rbuf_empty, df->stat, df->data_len);
        dumpbuf (df->bufhdr, len);

        // Give it to USB
        memcpy (dst, df->bufhdr, len);
        // Mark buffer as handled
        df->stat = BUF_EMPTY;
        rbuf_empty = (rbuf_empty + 1) % RBUFS;
    }
    return len;
}

// Check for a transmit done that transmit is waiting for.  The
// transmit request handler sets a flag if there are no free transmit
// buffers when a transmit request arrives.  Here we check if that
// flag is set and the number of pending transmits has dropped below
// the max.  If yes, tell the USB machinery that it can submit the
// rejected buffer again.
static void handle_tdone (void)
{
    if (tx_full && tbuf_count != TBUFS)
    {
        DPRINTF ("No longer full\n");
        tx_full = false;
        if (!bist)
        {
            tud_network_recv_renew ();
        }
    }
}

void tud_network_init_cb(void)
{
    // Nothing to do yet
}

// Unused, here to satisfy a reference in net_device.c
void rndis_class_set_handler(uint8_t *data, int size)
{
}

uint8_t bist_data[500] = {
    DLE, (sizeof (bist_data) - 6) & 0xff, (sizeof (bist_data) - 6) >> 8
};
uint32_t bist_seq;

static void send_bist_data (void)
{
    if (!tx_full)
    {
        memcpy (bist_data + 6, &bist_seq, 4);
        memcpy (bist_data + 3, &bist_seq, 2);
        if (transmit (bist_data, sizeof (bist_data)))
        {
            bist_seq++;
        }
    }
}

const struct command_msg_t bist_on =
{
    DLE, REQ_START, INT_LOOPBACK /*| INT_MODEM*/, 56000
};

// One-time initialization
static void init_once (void)
{
    int i, j;

    DPRINTF ("Initializing status buffer\n");
    memcpy (&status_msg, default_hdr, sizeof (default_hdr));
    status_msg.dc1 = DC1;
    status_msg.stat = DDCMP_OK;
    status_msg.on = false;
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
        // Set up the sync bytes
        for (j = 0; j < SYN_CNT; j++)
        {
            tbuf_ring[i].syn[j] = SYN;
        }
    }
    
    // Get a DMA channel
    txdma = dma_claim_unused_channel (true);
    txcsr = dma_channel_hw_addr (txdma);

    // Set interrupt handler
    irq_set_exclusive_handler (DMA_IRQ_0, dma_done);
    
    // Now initialize everything else
    stop_ddcmp ();
    
    DPRINTF ("One-time init done\n");
}

int main ()
{
#ifdef DEBUG
    stdio_init_all ();
#endif
    DPRINTF ("Initializing DDCMP framer\n");
    
    init_once ();

    /* initialize TinyUSB */
    if (bist)
    {
        command (&bist_on, sizeof (bist_on));
    }
    else
    {
        DPRINTF ("Initializing USB\n");
        tusb_init();
        send_status = true;
    }

    DPRINTF ("looking for received data\n");
    while (1) 
    {
        if (bist)
        {
            send_bist_data ();
        }
        else
        {
            tud_task();
        }
        handle_rbuf ();
        handle_tdone ();
    }
    
    return 0;
}