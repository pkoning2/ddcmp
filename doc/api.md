# DDCMP Framer application interface specification

The framer appears to the host as an Ethernet interface.  The interface hardware address is hardcoded aa-00-03-04-05-06 (an unused address from a range assigned to DEC and then rendered obsolete by IEEE 802 -- see `ether-i.pdf`).  The framer itself then appears as another Ethernet node connected to that NIC, with hardcoded address aa-00-03-04-05-07.

Communication with the framer uses raw Ethenet packets with protocol type is 60-06 (DEC protocol type assigned to "Customer use").

The payload of these Ethernet packets is either a DDCMP frame, or a command/status packet.  The encoding is slightly different for inbound (received data to host) vs. outbound (transmitted data from host).

Outbound payloads are simply DDCMP frames as encoded on the wire, except that the CRC fields are omitted (so the DDCMP header is only 6 bytes).  The framer supplies header and (if applicable) data CRC fields, as well as leading SYN and trailing DEL bytes.

Inbound payloads begin with a 2-byte receive status value, followed by the received DDCMP frame exactly as received, including the CRC fields.  Note that the framer implements CRC checking; the host normally would not look at the CRC bytes but simply use the status value to learn whether the packet is good or bad.

In either direction, we ignore the minimum frame length of Ethernet; only the needed bytes are sent since the USB emulated Ethernet NIC does not appear to object to undersized frames.  But since we limit framer packets to standard Ethernet size, the largest DDCMP data length supported is 1488.

Command/status packets begin with 021 (DC1) which is not a DDCMP frame start value.  Outbound they carry commands to the adapter; inbound they carry status and counters.  Note that the inbound status packet is preceded by a receive status word, as always; for status packets the receive status is always zero (OK).

Receive status word values:
0: good frame
1: Header CRC error
2: Data CRC error
3: Frame too long (if so, the entire data field is absent)

Command packets start with 021.  The next byte is the command:
0: status request
1: start
2: stop
3: send raw data
Status, start, and stop commands produce a status packet in reply (the status request simply is a "no state change" command).  "Send raw data" commands do not for valid requests, but if the command is rejected for any reason a status is generated to show this. For "send raw data", the data to be sent is the remainder of the command packet, starting immediately ater the command byte.  The data length must be a multiple of four.

For start, configuration settings follow:
Mode and flag bits (2 bytes).  Mode is the bottom 2 bits:

0: RS-232 DTE mode (bit clocks supplied by the connected modem)

1: Integral modem

2: RS-232 DCE mode (bit clock generated by framer)

Flags:

Bit 2: Internal loopback.  If mode is supplied as 0, it is forced to 2.

Bit 3: Built-in selftest.  In this mode, transmit requests are ignored and
received data is counted but not delivered.  The framer internally
generates a stream of DDCMP messages (500 bytes long) to exercise the
transmit and receive data paths.  Status requests still work so
progress can be monitored via status requests, and a stop command will
terminate the test.

Bit 4: Split speed.  This applies only to the integral modem case.  If set, the transmit speed is taken from the txspeed field rather than the speed field.  This is a test function that allows verifying the correct operation of receive data recovery when the sending end data rate is out of spec.

Speed: (4 bytes):

This specifies the bit rate in bits per second.  For RS-232 DCE mode, it is the frequency of the receive and transmit clock signals.  For integral modem, it is the transmit speed and the nominal receive speed; actual receive clocking is from the incoming data, which must match the specified rate +/- 6%.  Speed is not applicable for RS-232 DTE mode, in which the bit clocking is entirely under control of the modem.  Note that RS-232 is not officially specified for high speeds; it works well at 56 kbps with short cables, and can probably be made to work at 250 kbps, but it is unlikely to run at 1 Mbps since the required rise/fall times are too slow for that.  Standard integral modem speeds go as high as 1 Mbps; the adapter can go faster but standard DEC hardware does not; also, at some higher speeds the quantization error of the PIO clock divider becomes a problem and receive data recovery no longer works.

Transmit speed (4 bytes):

This is the transmit data rate, if the "split speed" bit is set in `mflags`.

Status packet starts with 021 (after the usual 2 byte receive status field), followed by state and counter data:

* state (1 byte): flags, bit 0 = on (active), bit 1 = in-sync
* mode and flags (2 bytes): as described above for the Start command
* speed (4 bytes): as described above.  Not used for RS-232 DTE mode.
* transmit speed (4 bytes): as described above.  
* Counters -- all are 4 bytes and wrap on overflow:
* received good frames
* transmitted frames
* received good bytes
* transmitted bytes
* header crc errors
* data crc errors
* frame too long errors
* buffer unavailable errors
* Additional status:
* last command status (4 bytes), 0 for good, non-zero for various errors
* measured data rate (4 bytes), in bits per second
* framer firmware version (null-terminated ASCII string)

Note that the measured data rate for the integral modem case is an approximation since it operates by measuring the edge timing of the received waveform.  The calculation assumes that the received data is half ones and half zeroes, which is the case for an idle line (SYN bytes are half and half).  If the line is actively carrying data and that data has a different bit density the measured line rate will be inaccurate; in particular for all zeroes it will be 33% higher than the true value.  The intent of this data item is to allow checking the data rate of a connected device whose clock may be off spec or unknown, allowing the framer to be configured with a matching data rate for reliable operation.

Last command status values are:

0: OK (command accepted)

1: Command message too short (must be at least 2 bytes for status or stop, at least 8 bytes for start).

2: Unrecognized command code (byte after the 0x11 command marker)

3: Start command issued when the device is already started

4: Speed out of range.  Supported speeds are 500 to 1,000,000 bits per second.

5: Bad transmit length.  For regular transmits, this means a control frame (starts with 0x05) less than 6 bytes long, or a data frame (starts with 0x81 or 0x90) with length in the header either 0 or greater than 1488, or not enough data in the Ethernet packet for the length given in the header.  For raw transmit, moer than 1488 bytes or not a multiple of 4.

6: Transmit request when the framer is stopped.

Note that transmit attempts in BIST mode are not errors; they are simply ignored.

## Diagnostics

### Built-in self test

If the framer is powered up with a jumper installed on J4 (marked "TEST"), it enters BIST mode in integral modem mode, internal loopback, 1 Mb/s.  Note that internal loopback of any kind is not dependent on the transceiver hardware, so it works even if the corresponding external connection option is not installed on the board.  In BIST mode the device continuously sends 500 byte test data frames, which are received and counted but not otherwise processed.  Every 1000 transmitted frames a status message is generated and sent to the host, which can be used to monitor the progress of the BIST.  BIST mode continues until the framer is powered down or until a "stop" command is issued.

BIST mode can also requested by setting bit 3 in the mode/flags field of a "Start" command.  In this case, the operating mode and loopback or not are selected by the other bits in the field.  So, for example, if a loopback connector is installed on the RS-232 DCE connector, a test including the RS-232 transceivers can be done by requesting RS-232 DCE mode, a suitable speed (250,000 should work) and BIST.

## Device validation and diagnostic test suite

The DECnet/Python package, in the `tests` directory, contains a set of framer tests in `test_framer.py`.  These can be invoked in the usual way, either the full set or any desired specific tests or test classes.  Refer to the Python library manual, "unittest" module, for details.  Some of the tests require loopback connectors to be installed, and some that use RS-232 DTE mode also require a clock source such as a modem eliminator to be attached to the DTE connector.  If this is not done those tests will fail with messages indicating the reason is a missing loopback connector or clock source.

The full set of framer tests require about TBD minutes to run.