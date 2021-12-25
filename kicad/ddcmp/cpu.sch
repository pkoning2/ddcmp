EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 4
Title "DDCMP Framer"
Date "2021-05-17"
Rev "A"
Comp "AK Design"
Comment1 "Copyright © 2021 by Paul Koning, NI1D"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	5250 2750 5050 2750
Wire Wire Line
	5050 2750 5050 3250
$Comp
L power:GND #PWR0101
U 1 1 60A51795
P 5050 4750
F 0 "#PWR0101" H 5050 4500 50  0001 C CNN
F 1 "GND" H 5055 4577 50  0000 C CNN
F 2 "" H 5050 4750 50  0001 C CNN
F 3 "" H 5050 4750 50  0001 C CNN
	1    5050 4750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 60A5194C
P 6850 4750
F 0 "#PWR0102" H 6850 4500 50  0001 C CNN
F 1 "GND" H 6855 4577 50  0000 C CNN
F 2 "" H 6850 4750 50  0001 C CNN
F 3 "" H 6850 4750 50  0001 C CNN
	1    6850 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 3250 6850 3250
Wire Wire Line
	6850 3250 6850 3750
Wire Wire Line
	6650 3750 6850 3750
Connection ~ 6850 3750
Wire Wire Line
	6850 3750 6850 4250
Wire Wire Line
	6650 4250 6850 4250
Connection ~ 6850 4250
Wire Wire Line
	6850 4250 6850 4750
Wire Wire Line
	5250 3250 5050 3250
Connection ~ 5050 3250
Wire Wire Line
	5050 3250 5050 3750
Wire Wire Line
	5250 3750 5050 3750
Connection ~ 5050 3750
Wire Wire Line
	5050 3750 5050 4250
Wire Wire Line
	5250 4250 5050 4250
Connection ~ 5050 4250
Wire Wire Line
	5050 4250 5050 4750
Text GLabel 7200 2950 2    50   UnSpc ~ 0
V3_3
$Comp
L power:GND #PWR0103
U 1 1 60A52F43
P 5950 4800
F 0 "#PWR0103" H 5950 4550 50  0001 C CNN
F 1 "GND" H 5955 4627 50  0000 C CNN
F 2 "" H 5950 4800 50  0001 C CNN
F 3 "" H 5950 4800 50  0001 C CNN
	1    5950 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 4800 5950 4650
$Comp
L Connector_Generic:Conn_01x06 J1
U 1 1 60AB089C
P 3100 2350
F 0 "J1" H 3018 2767 50  0000 C CNN
F 1 "UART" H 3018 2676 50  0000 C CNN
F 2 "ddcmp_parts:Pin_Header_Straight_1x06_Pitch2.54mm" H 3100 2350 50  0001 C CNN
F 3 "https://cdn.harwin.com/pdfs/C001XX_M20_Series_Connectors.pdf" H 3100 2350 50  0001 C CNN
F 4 "952-2269-ND" H 3100 2350 50  0001 C CNN "Digikey part"
F 5 "Harwin Inc." H 3100 2350 50  0001 C CNN "Vendor"
F 6 "M20-9990645" H 3100 2350 50  0001 C CNN "Vendor part"
	1    3100 2350
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5050 2450 5050 2650
Wire Wire Line
	5050 2650 5250 2650
Text Notes 3300 2050 0    50   ~ 0
Debug connector\n(optional)
Wire Wire Line
	3300 2450 5050 2450
Wire Wire Line
	3300 2550 5250 2550
$Comp
L power:GND #PWR0108
U 1 1 60AB5E1B
P 3800 2250
F 0 "#PWR0108" H 3800 2000 50  0001 C CNN
F 1 "GND" H 3805 2077 50  0000 C CNN
F 2 "" H 3800 2250 50  0001 C CNN
F 3 "" H 3800 2250 50  0001 C CNN
	1    3800 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 2250 3800 2150
Wire Wire Line
	3800 2150 3300 2150
$Comp
L Device:LED D1
U 1 1 60AC703D
P 8350 3800
F 0 "D1" V 8389 3880 50  0000 L CNN
F 1 "SYN" V 8298 3880 50  0000 L CNN
F 2 "ddcmp_parts:LED_D3.0mm" H 8350 3800 50  0001 C CNN
F 3 "https://www.sunledusa.com/products/spec/XLUG34D.pdf" H 8350 3800 50  0001 C CNN
F 4 "1497-1490-ND" H 8350 3800 50  0001 C CNN "Digikey part"
F 5 "SunLED" H 8350 3800 50  0001 C CNN "Vendor"
F 6 "XLUG34D" H 8350 3800 50  0001 C CNN "Vendor part"
	1    8350 3800
	0    1    -1   0   
$EndComp
$Comp
L Device:LED D2
U 1 1 60AC83AC
P 8700 3800
F 0 "D2" V 8739 3880 50  0000 L CNN
F 1 "RX" V 8648 3880 50  0000 L CNN
F 2 "ddcmp_parts:LED_D3.0mm" H 8700 3800 50  0001 C CNN
F 3 "https://www.sunledusa.com/products/spec/XLUY34D.pdf" H 8700 3800 50  0001 C CNN
F 4 "1497-1492-ND" H 8700 3800 50  0001 C CNN "Digikey part"
F 5 "SunLED" H 8700 3800 50  0001 C CNN "Vendor"
F 6 "XLUY34D" H 8700 3800 50  0001 C CNN "Vendor part"
	1    8700 3800
	0    1    -1   0   
$EndComp
$Comp
L Device:LED D3
U 1 1 60AC89D4
P 9050 3800
F 0 "D3" V 9089 3880 50  0000 L CNN
F 1 "TX" V 8998 3880 50  0000 L CNN
F 2 "ddcmp_parts:LED_D3.0mm" H 9050 3800 50  0001 C CNN
F 3 "https://www.sunledusa.com/products/spec/XLUR34D.pdf" H 9050 3800 50  0001 C CNN
F 4 "1497-1491-ND" H 9050 3800 50  0001 C CNN "Digikey part"
F 5 "SunLED" H 9050 3800 50  0001 C CNN "Vendor"
F 6 "XLUR34D" H 9050 3800 50  0001 C CNN "Vendor part"
	1    9050 3800
	0    1    -1   0   
$EndComp
$Comp
L ddcmp_symbols:R_US R3
U 1 1 60ACA428
P 8350 4200
F 0 "R3" H 8418 4246 50  0000 L CNN
F 1 "150" H 8418 4155 50  0000 L CNN
F 2 "ddcmp_parts:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 8390 4190 50  0001 C CNN
F 3 "https://www.yageo.com/upload/media/product/products/datasheet/lr/Yageo_LR_CFR_1.pdf" H 8350 4200 50  0001 C CNN
F 4 "150QBK-ND" H 8350 4200 50  0001 C CNN "Digikey part"
F 5 "Yageo" H 8350 4200 50  0001 C CNN "Vendor"
F 6 "CFR-25JB-52-150R" H 8350 4200 50  0001 C CNN "Vendor part"
	1    8350 4200
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:R_US R4
U 1 1 60ACAA45
P 8700 4200
F 0 "R4" H 8768 4246 50  0000 L CNN
F 1 "150" H 8768 4155 50  0000 L CNN
F 2 "ddcmp_parts:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 8740 4190 50  0001 C CNN
F 3 "https://www.yageo.com/upload/media/product/products/datasheet/lr/Yageo_LR_CFR_1.pdf" H 8700 4200 50  0001 C CNN
F 4 "150QBK-ND" H 8700 4200 50  0001 C CNN "Digikey part"
F 5 "Yageo" H 8700 4200 50  0001 C CNN "Vendor"
F 6 "CFR-25JB-52-150R" H 8700 4200 50  0001 C CNN "Vendor part"
	1    8700 4200
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:R_US R5
U 1 1 60ACB075
P 9050 4200
F 0 "R5" H 9118 4246 50  0000 L CNN
F 1 "150" H 9118 4155 50  0000 L CNN
F 2 "ddcmp_parts:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 9090 4190 50  0001 C CNN
F 3 "https://www.yageo.com/upload/media/product/products/datasheet/lr/Yageo_LR_CFR_1.pdf" H 9050 4200 50  0001 C CNN
F 4 "150QBK-ND" H 9050 4200 50  0001 C CNN "Digikey part"
F 5 "Yageo" H 9050 4200 50  0001 C CNN "Vendor"
F 6 "CFR-25JB-52-150R" H 9050 4200 50  0001 C CNN "Vendor part"
	1    9050 4200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0111
U 1 1 60ACBBD7
P 8350 4450
F 0 "#PWR0111" H 8350 4200 50  0001 C CNN
F 1 "GND" H 8355 4277 50  0000 C CNN
F 2 "" H 8350 4450 50  0001 C CNN
F 3 "" H 8350 4450 50  0001 C CNN
	1    8350 4450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0112
U 1 1 60ACC03F
P 8700 4450
F 0 "#PWR0112" H 8700 4200 50  0001 C CNN
F 1 "GND" H 8705 4277 50  0000 C CNN
F 2 "" H 8700 4450 50  0001 C CNN
F 3 "" H 8700 4450 50  0001 C CNN
	1    8700 4450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0113
U 1 1 60ACC9AB
P 9050 4450
F 0 "#PWR0113" H 9050 4200 50  0001 C CNN
F 1 "GND" H 9055 4277 50  0000 C CNN
F 2 "" H 9050 4450 50  0001 C CNN
F 3 "" H 9050 4450 50  0001 C CNN
	1    9050 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 4450 9050 4350
Wire Wire Line
	8700 4450 8700 4350
Wire Wire Line
	8350 4450 8350 4350
Wire Wire Line
	8350 4050 8350 3950
Wire Wire Line
	8700 4050 8700 3950
Wire Wire Line
	9050 4050 9050 3950
Wire Wire Line
	6650 4350 7200 4350
Wire Wire Line
	6650 4150 7200 4150
Wire Wire Line
	5250 3850 4750 3850
Wire Wire Line
	5250 3950 4750 3950
Wire Wire Line
	5250 4050 4750 4050
Wire Wire Line
	5250 4150 4750 4150
Wire Wire Line
	5250 4350 4750 4350
Wire Wire Line
	5250 4450 4750 4450
Text HLabel 7200 4150 2    50   Output ~ 0
IM_ENABLE
Text HLabel 7200 4350 2    50   Output ~ 0
IM_TXDATA
Text HLabel 7200 4050 2    50   Input ~ 0
IM_RXDATA
Text HLabel 4750 3850 0    50   Output ~ 0
RS232_ENABLE
Text HLabel 4750 3950 0    50   Output ~ 0
RS232_CLKOUT
Text HLabel 4750 4050 0    50   Input ~ 0
RS232_TXCLK
Text HLabel 4750 4150 0    50   Input ~ 0
RS232_RXDATA
Text HLabel 4750 4350 0    50   Input ~ 0
RS232_RXCLK
Text HLabel 4750 4450 0    50   Output ~ 0
RS232_TXDATA
$Comp
L Connector_Generic:Conn_01x02 J4
U 1 1 60AFBDD5
P 4150 2750
F 0 "J4" H 4230 2742 50  0000 L CNN
F 1 "TEST" H 4230 2651 50  0000 L CNN
F 2 "ddcmp_parts:Pin_Header_Straight_1x02_Pitch2.54mm" H 4150 2750 50  0001 C CNN
F 3 "https://cdn.harwin.com/pdfs/C001XX_M20_Series_Connectors.pdf" H 4150 2750 50  0001 C CNN
F 4 "952-2261-ND" H 4150 2750 50  0001 C CNN "Digikey part"
F 5 "Harwin Inc." H 4150 2750 50  0001 C CNN "Vendor"
F 6 "M20-9990245" H 4150 2750 50  0001 C CNN "Vendor part"
	1    4150 2750
	-1   0    0    -1  
$EndComp
NoConn ~ 3300 2250
NoConn ~ 3300 2350
NoConn ~ 3300 2650
NoConn ~ 5250 3150
NoConn ~ 6650 2850
NoConn ~ 6650 3050
NoConn ~ 6650 3550
NoConn ~ 5100 4700
NoConn ~ 5850 4650
NoConn ~ 6050 4650
$Comp
L ddcmp_symbols:Raspberry_Pico U?
U 1 1 60A47CFF
P 5950 3500
AR Path="/60A47CFF" Ref="U?"  Part="1" 
AR Path="/60A46E24/60A47CFF" Ref="U1"  Part="1" 
F 0 "U1" H 5950 4715 50  0000 C CNN
F 1 "Pico" H 5950 4624 50  0000 C CNN
F 2 "ddcmp_parts:RPi_Pico_SMD_TH" V 5950 3500 50  0001 C CNN
F 3 "https://datasheets.raspberrypi.org/pico/pico-datasheet.pdf" H 5950 3500 50  0001 C CNN
F 4 "2648-SC0915CT-ND " H 5950 3500 50  0001 C CNN "Digikey part"
F 5 "Raspberry Pi" H 5950 3500 50  0001 C CNN "Vendor"
F 6 "SC0915" H 5950 3500 50  0001 C CNN "Vendor part"
	1    5950 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 2950 7200 2950
Wire Wire Line
	7200 4050 6650 4050
Wire Wire Line
	5050 2750 4350 2750
Connection ~ 5050 2750
Wire Wire Line
	4350 2850 5250 2850
Wire Wire Line
	6650 3450 8350 3450
Wire Wire Line
	8350 3450 8350 3650
Wire Wire Line
	8700 3650 8700 3350
Wire Wire Line
	8700 3350 6650 3350
Wire Wire Line
	6650 3150 9050 3150
Wire Wire Line
	9050 3150 9050 3650
NoConn ~ 5250 3050
NoConn ~ 5250 2950
NoConn ~ 6650 3650
NoConn ~ 6650 3850
Wire Wire Line
	6650 2750 6850 2750
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 60B1F675
P 7200 2750
F 0 "#FLG0101" H 7200 2825 50  0001 C CNN
F 1 "PWR_FLAG" H 7200 2923 50  0000 C CNN
F 2 "" H 7200 2750 50  0001 C CNN
F 3 "~" H 7200 2750 50  0001 C CNN
	1    7200 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	6850 3250 6850 2750
Connection ~ 6850 3250
Connection ~ 6850 2750
Wire Wire Line
	6850 2750 7200 2750
Wire Wire Line
	6650 2550 6750 2550
Wire Wire Line
	6750 2550 6750 2650
Wire Wire Line
	6750 2650 6650 2650
Text Notes 6750 2500 0    50   ~ 0
VBus connected to Vsys for efficiency.\nSee Pico data sheet, section 4.5
Wire Wire Line
	4850 3350 5250 3350
Wire Wire Line
	4850 3450 5250 3450
Wire Wire Line
	4850 3550 5250 3550
NoConn ~ 4850 3350
NoConn ~ 4850 3450
NoConn ~ 4850 3550
Text Notes 4800 3350 2    50   ~ 0
Loopback clock
Text Notes 4800 3450 2    50   ~ 0
Loopback data
Text Notes 4800 3550 2    50   ~ 0
Integral modem recovered data
Text Notes 4150 3000 2    50   ~ 0
Install jumper for power on BIST
Wire Wire Line
	5250 3650 4850 3650
NoConn ~ 4850 3650
Text Notes 4800 3650 2    50   ~ 0
Integral modem receive strobe
Text Notes 4950 3250 2    50   ~ 0
The four pins listed below, as well as pin 21 (GPIO16) are no-connect, but \nthey can be used as test points to observe internal status of the framer. 
Text Notes 6850 6150 0    100  ~ 0
Note:\n1. J1 can be omitted if the debug UART is not needed.\n2. J4 can be omitted if power on BIST is not needed.\n3. D1, D2, and/or D3 and R3, R4, and/or R5 can be\n   omitted if the corresponding indicator is not needed.
Wire Wire Line
	6650 4450 7150 4450
NoConn ~ 7150 4450
Text Notes 7200 4500 0    50   ~ 0
Clock ok
NoConn ~ 6650 3950
$EndSCHEMATC
