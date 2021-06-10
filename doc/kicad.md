# DDCMP Framer

The `kicad/ddcmp` subdirectory contains the design files for the DDCMP framer circuit board, which you can inspect or modify using the open source KiCAD package.

## Creating the circuit board

There are three ways to get a circuit board.
1. From the shared designs section of the OSHPark PCB fabricator website, in [this link](https://oshpark.com/shared_projects/TO_BE_SUPPLIED "DDCMP Framer").
2. From any PCB fabricator that accepts KiCAD board files (such as OSHPark).
3. From any PCB fabricator that doesn't, by generating the standard *Gerber* files.

If you want to use a fabricator that needs Gerber files, open the `ddcmp.kicad_pcb` file using KiCAD (specifically the *Pcbnew* application) and use the File->Plot menu option to generate them.  You'll want to review the detailed instructions from the fabricator to confirm settings, file name rules, etc.

## Schematic

The board schematic is in `ddcmp.sch` (which is the top level of a hierarchical design pointing to three additional `*.sch` files).  You won't need this to create the board, but you can review this if you are interested, or if you want to modify the design.

## Assembly

The design is a simple circuit board; nearly all components are through-hole.  The one exception is the pair of Min-Circuits transformers, which are surface mount, but with 0.1 inch lead pitch so they do not create any problem.  If you have experience building small electronic kits it should not create any problems.  If you do not, you may want to seek out some help since this is not a "Heathkit" style kit with detailed instructions or tutorials.

## Parts list

Here are all the parts in the design, with schematic reference (also on the board silk screen labeling), value or device type, description, along with manufacturer name and Digikey part number.  The Digikey numbers are just by way of illustration.  Capacitors and resistors are generic, you can replace them by similar devices from other suppliers so long as they fit in the available space.  The capacitors  are small ceramic devices with 0.1 inch lead spacing; the resistors are standard 1/4 watt parts. 

| Ref | Value | Description | Manufacturer | Digikey part # |
| --- | ----- | ----------- | ------------ | -------------- |
| C1 | 0.1uF | Ceramic capacitor | Kemet | 399-14010-1-ND |
| C2 | 0.1uF | Ceramic capacitor | Kemet | 399-14010-1-ND |
| C3 | 0.1uF | Ceramic capacitor | Kemet | 399-14010-1-ND |
| C4 | 0.1uF | Ceramic capacitor | Kemet | 399-14010-1-ND |
| C5 | 0.1uF | Ceramic capacitor | Kemet | 399-14010-1-ND |
| C6 | 0.1uF | Ceramic capacitor | Kemet | 399-14010-1-ND |
| C7 | 0.1uF | Ceramic capacitor | Kemet | 399-14010-1-ND |
| C8 | 0.1uF | Ceramic capacitor | Kemet | 399-14010-1-ND |
| C9 | 0.1uF | Ceramic capacitor | Kemet | 399-14010-1-ND |
| C10 | 0.1uF | Ceramic capacitor | Kemet | 399-14010-1-ND |
| C11 | 0.1uF | Ceramic capacitor | Kemet | 399-14010-1-ND |
| D1 | SYN | 3 mm green LED | SunLED | 1497-1490-ND |
| D2 | RX | 3 mm yellow LED | SunLED | 1497-1492-ND |
| D3 | TX | 3 mm red LED | SunLED | 1497-1491-ND |
| J1 | UART | 6-pin header | Harwin Inc. | 952-2269-ND |
| J2 | RX | BNC connector | Molex | WM20430-ND |
| J3 | TX | BNC connector | Molex | WM20430-ND |
| J4 | TEST | 2-pin header | Harwin Inc. | 952-2261-ND |
| J5 | DTE | DB25 male connector | Amphenol ICC | L717SDB25P1ACH4F-ND |
| J6 | DCE | DB25 female connector | Amphenol ICC | L77SDB25S1ACH3R-ND |
| J7 | Bulkhead | "Berg" 40 pin connector | Amphenol ICC | 609-1779-ND |
| R1 | 1M | Carbon film resistor | Yageo | 1.0MQBK-ND |
| R2 | 1M | Carbon film resistor | Yageo | 1.0MQBK-ND |
| R3 | 150 | Carbon film resistor | Yageo | 150QBK-ND |
| R4 | 150 | Carbon film resistor | Yageo | 150QBK-ND |
| R5 | 150 | Carbon film resistor | Yageo | 150QBK-ND |
| R6 | 51 | Carbon film resistor | Yageo | 51QBK-ND |
| R7 | 51 | Carbon film resistor | Yageo | 51QBK-ND |
| TR1 | TT1-6-X65+ | Transformer | Mini-Circuits |  |
| TR2 | TT1-6-X65+ | Transformer | Mini-Circuits |  |
| U1 | Pico | Raspberry Pico | Raspberry Pi | 2648-SC0915CT-ND  |
| U2 | max3491ecpd | RS-422 transceiver | Maxim | MAX3491CPD+-ND |
| U3 | MAX3222 | RS-232 transceiver | Maxim | MAX3222ECPN+-ND |
| U4 | MAX3222 | RS-232 transceiver | Maxim | MAX3222ECPN+-ND |

If you want to use Digikey as your supplier, the saved BOM is available [there](https://www.digikey.com/short/TO_BE_SUPPLIED "Digikey BOM").  Note that TR1 and TR2 are not in that BOM since Digikey does not have these; you can order them directly from Mini-Circuits.

### Optional components

Apart from U1 (the processor) almost all the parts are optional; install them or not depending on which features you are interested in.  When in doubt, consult the schematic to see what each part does.

The options are:

1. RS-232 support: C1-C10, J4, J5, U3, U4.
2. RS-232 DTE support: J5.
3. RS-232 DCE support: J4.
4. Activity lights: omit D1, D2, and/or D3 with their associated resistors R3, R4, and/or R5 if you do not want the SYN, RX, and/or TX indicators.
5. Integral modem support: C11, J2, J3, R1, R2, R6, R7, TR1, TR2, U2.
6. Integral modem direct coax connection: J2, J3.
7. DEC BC55 bulkhead connection support: J7.
8. Debug UART connection: J1.
9. Easy access to built-in selftest feature: J4.

### Transformer alternatives

The listed part for TR1 and TR2 is the Mini-Circuits TT1-6-X65, which is a 6 pin DIP (through-hole) part.  It doesn't appear in the Digikey catalog, but it can be obtained directly from Mini-Circuits.

If you have trouble getting this part, several other Mini-Circuits transformers could be used instead.  The alternatives are all surface mount parts; the PCB footprint is designed to handle either the surface mount or the DIP part.

#### Tested alternatives

1. Mini-Circuits ADT1-6T+.  That one works fine at 1 Mbps but its low frequency limit makes it marginal at best at 56 kbps.

#### Untested alternatives

1. Mini-Circuits ADTT1-6+ appears to be suitable judging by its specifications.

2. Mini-Circuits ADT2-71T+ may work.  This is a 1:2 impedance transformer.  TheMAX3491 transceiver is designed for either 50 ohm or 100 ohm load so this should be fine, but the output voltage at the 50 ohm connector will be somewhat reduced.  So long as the cables are not too long I would not expect a problem.  If this option is used, replace R6 and R7 by 100 ohm resistors.  Also, with this part the correct orientation is important.
 
### Assembly suggestions

I found that this order of installing the components works well:
1. U1 (Raspberry Pico).  You can surface-mount this or use pin headers (or even short wires) if you prefer.
2. TR1, TR2 transformers, if you are using surface mount parts
3. Resistors and capacitors
4. LEDs
5. U2, U3, U4 DIP integrated circuits; also TR1 and TR2, if through-hole
6. J1, J4 pin headers
7. J2, J3, J5, J6, J7 connectors.

Note the correct orientation of D1, D2, D3, J7, U1, U2, U3, and U4.  U1 is oriented with the USB at the top of the board and all the contacts matched to pads on the board.  The other three ICs have a square pad at pin 1.  J7 (the 40-pin connector) has the keying notches towards the top of the board.  The LEDs have their flat side at the square pad.

The transformers TR1 and TR2 have an orientation dot on the package.  For the ADT2-71T+ orintation is important (see above).

## Software load

After the board is built you will need to load the software.  See the `software.md` file for instructions.
