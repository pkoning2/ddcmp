EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 4
Title "DDCMP Framer"
Date "2021-05-17"
Rev "A"
Comp "AK Design"
Comment1 "Copyright © 2021 by Paul Koning, NI1D"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ddcmp_symbols:MAX3222 U?
U 1 1 60A4D268
P 7600 2500
AR Path="/60A4D268" Ref="U?"  Part="1" 
AR Path="/60A47002/60A4D268" Ref="U4"  Part="1" 
F 0 "U4" H 7600 3881 50  0000 C CNN
F 1 "MAX3222" H 7600 3790 50  0000 C CNN
F 2 "ddcmp_parts:DIP-18_W7.62mm" H 7650 1450 50  0001 L CNN
F 3 "https://datasheets.maximintegrated.com/en/ds/MAX3222E-MAX3246E.pdf" H 7600 2600 50  0001 C CNN
F 4 "MAX3222ECPN+-ND" H 0   0   50  0001 C CNN "Digikey part"
F 5 "Maxim" H 0   0   50  0001 C CNN "Vendor"
F 6 "MAX3222ECPN" H 0   0   50  0001 C CNN "Vendor part"
	1    7600 2500
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:MAX3222 U?
U 1 1 60A4D270
P 3700 2500
AR Path="/60A4D270" Ref="U?"  Part="1" 
AR Path="/60A47002/60A4D270" Ref="U3"  Part="1" 
F 0 "U3" H 3700 3881 50  0000 C CNN
F 1 "MAX3222" H 3700 3790 50  0000 C CNN
F 2 "ddcmp_parts:DIP-18_W7.62mm" H 3750 1450 50  0001 L CNN
F 3 "https://datasheets.maximintegrated.com/en/ds/MAX3222E-MAX3246E.pdf" H 3700 2600 50  0001 C CNN
F 4 "MAX3222ECPN+-ND" H 3700 2500 50  0001 C CNN "Digikey part"
F 5 "Maxim" H 3700 2500 50  0001 C CNN "Vendor"
F 6 "MAX3222ECPN" H 3700 2500 50  0001 C CNN "Vendor part"
	1    3700 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	7600 1300 7600 800 
Wire Wire Line
	3700 1300 3700 850 
Wire Wire Line
	3700 850  2350 850 
Text GLabel 2150 850  0    50   UnSpc ~ 0
V3_3
Wire Wire Line
	5950 800  6150 800 
$Comp
L power:GND #PWR0106
U 1 1 60A60B52
P 7600 3900
F 0 "#PWR0106" H 7600 3650 50  0001 C CNN
F 1 "GND" H 7605 3727 50  0000 C CNN
F 2 "" H 7600 3900 50  0001 C CNN
F 3 "" H 7600 3900 50  0001 C CNN
	1    7600 3900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0107
U 1 1 60A60D47
P 3700 3800
F 0 "#PWR0107" H 3700 3550 50  0001 C CNN
F 1 "GND" H 3705 3627 50  0000 C CNN
F 2 "" H 3700 3800 50  0001 C CNN
F 3 "" H 3700 3800 50  0001 C CNN
	1    3700 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 3700 3700 3800
Wire Wire Line
	7600 3900 7600 3700
$Comp
L ddcmp_symbols:C C2
U 1 1 60A756A3
P 2600 1750
F 0 "C2" H 2715 1796 50  0000 L CNN
F 1 "0.1uF" H 2715 1705 50  0000 L CNN
F 2 "ddcmp_parts:C_Rect_L4.0mm_W2.5mm_P2.54mm" H 2638 1600 50  0001 C CNN
F 3 "" H 2600 1750 50  0001 C CNN
F 4 "399-14010-1-ND" H 2600 1750 50  0001 C CNN "Digikey part"
F 5 "Kemet" H 2600 1750 50  0001 C CNN "Vendor"
F 6 "C315C104M5U5TA7301" H 2600 1750 50  0001 C CNN "Vendor part"
F 7 "399-14010-1-ND" H 2600 1750 50  0001 C CNN "Digikey part"
F 8 "Kemet" H 2600 1750 50  0001 C CNN "Vendor"
F 9 "C315C104M5U5TA7301" H 2600 1750 50  0001 C CNN "Vendor part"
	1    2600 1750
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:C C3
U 1 1 60A76079
P 4800 1750
F 0 "C3" H 4915 1796 50  0000 L CNN
F 1 "0.1uF" H 4915 1705 50  0000 L CNN
F 2 "ddcmp_parts:C_Rect_L4.0mm_W2.5mm_P2.54mm" H 4838 1600 50  0001 C CNN
F 3 "" H 4800 1750 50  0001 C CNN
F 4 "399-14010-1-ND" H 4800 1750 50  0001 C CNN "Digikey part"
F 5 "Kemet" H 4800 1750 50  0001 C CNN "Vendor"
F 6 "C315C104M5U5TA7301" H 4800 1750 50  0001 C CNN "Vendor part"
F 7 "399-14010-1-ND" H 4800 1750 50  0001 C CNN "Digikey part"
F 8 "Kemet" H 4800 1750 50  0001 C CNN "Vendor"
F 9 "C315C104M5U5TA7301" H 4800 1750 50  0001 C CNN "Vendor part"
	1    4800 1750
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:C C7
U 1 1 60A76533
P 6400 1750
F 0 "C7" H 6515 1796 50  0000 L CNN
F 1 "0.1uF" H 6515 1705 50  0000 L CNN
F 2 "ddcmp_parts:C_Rect_L4.0mm_W2.5mm_P2.54mm" H 6438 1600 50  0001 C CNN
F 3 "" H 6400 1750 50  0001 C CNN
F 4 "399-14010-1-ND" H 6400 1750 50  0001 C CNN "Digikey part"
F 5 "Kemet" H 6400 1750 50  0001 C CNN "Vendor"
F 6 "C315C104M5U5TA7301" H 6400 1750 50  0001 C CNN "Vendor part"
F 7 "399-14010-1-ND" H 6400 1750 50  0001 C CNN "Digikey part"
F 8 "Kemet" H 6400 1750 50  0001 C CNN "Vendor"
F 9 "C315C104M5U5TA7301" H 6400 1750 50  0001 C CNN "Vendor part"
	1    6400 1750
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:C C8
U 1 1 60A76897
P 8700 1750
F 0 "C8" H 8815 1796 50  0000 L CNN
F 1 "0.1uF" H 8815 1705 50  0000 L CNN
F 2 "ddcmp_parts:C_Rect_L4.0mm_W2.5mm_P2.54mm" H 8738 1600 50  0001 C CNN
F 3 "" H 8700 1750 50  0001 C CNN
F 4 "399-14010-1-ND" H 8700 1750 50  0001 C CNN "Digikey part"
F 5 "Kemet" H 8700 1750 50  0001 C CNN "Vendor"
F 6 "C315C104M5U5TA7301" H 8700 1750 50  0001 C CNN "Vendor part"
F 7 "399-14010-1-ND" H 8700 1750 50  0001 C CNN "Digikey part"
F 8 "Kemet" H 8700 1750 50  0001 C CNN "Vendor"
F 9 "C315C104M5U5TA7301" H 8700 1750 50  0001 C CNN "Vendor part"
	1    8700 1750
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:C C1
U 1 1 60A77B7B
P 2350 1150
F 0 "C1" H 2465 1196 50  0000 L CNN
F 1 "0.1uF" H 2465 1105 50  0000 L CNN
F 2 "ddcmp_parts:C_Rect_L4.0mm_W2.5mm_P2.54mm" H 2388 1000 50  0001 C CNN
F 3 "" H 2350 1150 50  0001 C CNN
F 4 "399-14010-1-ND" H 2350 1150 50  0001 C CNN "Digikey part"
F 5 "Kemet" H 2350 1150 50  0001 C CNN "Vendor"
F 6 "C315C104M5U5TA7301" H 2350 1150 50  0001 C CNN "Vendor part"
F 7 "399-14010-1-ND" H 2350 1150 50  0001 C CNN "Digikey part"
F 8 "Kemet" H 2350 1150 50  0001 C CNN "Vendor"
F 9 "C315C104M5U5TA7301" H 2350 1150 50  0001 C CNN "Vendor part"
	1    2350 1150
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:C C6
U 1 1 60A79146
P 6150 1150
F 0 "C6" H 6265 1196 50  0000 L CNN
F 1 "0.1uF" H 6265 1105 50  0000 L CNN
F 2 "ddcmp_parts:C_Rect_L4.0mm_W2.5mm_P2.54mm" H 6188 1000 50  0001 C CNN
F 3 "" H 6150 1150 50  0001 C CNN
F 4 "399-14010-1-ND" H 6150 1150 50  0001 C CNN "Digikey part"
F 5 "Kemet" H 6150 1150 50  0001 C CNN "Vendor"
F 6 "C315C104M5U5TA7301" H 6150 1150 50  0001 C CNN "Vendor part"
F 7 "399-14010-1-ND" H 6150 1150 50  0001 C CNN "Digikey part"
F 8 "Kemet" H 6150 1150 50  0001 C CNN "Vendor"
F 9 "C315C104M5U5TA7301" H 6150 1150 50  0001 C CNN "Vendor part"
	1    6150 1150
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:C C5
U 1 1 60A798E1
P 5650 2600
F 0 "C5" H 5765 2646 50  0000 L CNN
F 1 "0.1uF" H 5765 2555 50  0000 L CNN
F 2 "ddcmp_parts:C_Rect_L4.0mm_W2.5mm_P2.54mm" H 5688 2450 50  0001 C CNN
F 3 "" H 5650 2600 50  0001 C CNN
F 4 "399-14010-1-ND" H 5650 2600 50  0001 C CNN "Digikey part"
F 5 "Kemet" H 5650 2600 50  0001 C CNN "Vendor"
F 6 "C315C104M5U5TA7301" H 5650 2600 50  0001 C CNN "Vendor part"
F 7 "399-14010-1-ND" H 5650 2600 50  0001 C CNN "Digikey part"
F 8 "Kemet" H 5650 2600 50  0001 C CNN "Vendor"
F 9 "C315C104M5U5TA7301" H 5650 2600 50  0001 C CNN "Vendor part"
	1    5650 2600
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:C C4
U 1 1 60A7AB61
P 5250 2600
F 0 "C4" H 5365 2646 50  0000 L CNN
F 1 "0.1uF" H 5365 2555 50  0000 L CNN
F 2 "ddcmp_parts:C_Rect_L4.0mm_W2.5mm_P2.54mm" H 5288 2450 50  0001 C CNN
F 3 "" H 5250 2600 50  0001 C CNN
F 4 "399-14010-1-ND" H 5250 2600 50  0001 C CNN "Digikey part"
F 5 "Kemet" H 5250 2600 50  0001 C CNN "Vendor"
F 6 "C315C104M5U5TA7301" H 5250 2600 50  0001 C CNN "Vendor part"
F 7 "399-14010-1-ND" H 5250 2600 50  0001 C CNN "Digikey part"
F 8 "Kemet" H 5250 2600 50  0001 C CNN "Vendor"
F 9 "C315C104M5U5TA7301" H 5250 2600 50  0001 C CNN "Vendor part"
	1    5250 2600
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:C C9
U 1 1 60A7AF1F
P 9100 2600
F 0 "C9" H 9215 2646 50  0000 L CNN
F 1 "0.1uF" H 9215 2555 50  0000 L CNN
F 2 "ddcmp_parts:C_Rect_L4.0mm_W2.5mm_P2.54mm" H 9138 2450 50  0001 C CNN
F 3 "" H 9100 2600 50  0001 C CNN
F 4 "399-14010-1-ND" H 9100 2600 50  0001 C CNN "Digikey part"
F 5 "Kemet" H 9100 2600 50  0001 C CNN "Vendor"
F 6 "C315C104M5U5TA7301" H 9100 2600 50  0001 C CNN "Vendor part"
F 7 "399-14010-1-ND" H 9100 2600 50  0001 C CNN "Digikey part"
F 8 "Kemet" H 9100 2600 50  0001 C CNN "Vendor"
F 9 "C315C104M5U5TA7301" H 9100 2600 50  0001 C CNN "Vendor part"
	1    9100 2600
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:C C10
U 1 1 60A7B4DD
P 9700 2600
F 0 "C10" H 9815 2646 50  0000 L CNN
F 1 "0.1uF" H 9815 2555 50  0000 L CNN
F 2 "ddcmp_parts:C_Rect_L4.0mm_W2.5mm_P2.54mm" H 9738 2450 50  0001 C CNN
F 3 "" H 9700 2600 50  0001 C CNN
F 4 "399-14010-1-ND" H 9700 2600 50  0001 C CNN "Digikey part"
F 5 "Kemet" H 9700 2600 50  0001 C CNN "Vendor"
F 6 "C315C104M5U5TA7301" H 9700 2600 50  0001 C CNN "Vendor part"
F 7 "399-14010-1-ND" H 9700 2600 50  0001 C CNN "Digikey part"
F 8 "Kemet" H 9700 2600 50  0001 C CNN "Vendor"
F 9 "C315C104M5U5TA7301" H 9700 2600 50  0001 C CNN "Vendor part"
	1    9700 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2350 1000 2350 850 
Connection ~ 2350 850 
Wire Wire Line
	2350 850  2150 850 
Wire Wire Line
	6150 1000 6150 800 
Connection ~ 6150 800 
Wire Wire Line
	6150 800  7600 800 
Wire Wire Line
	6800 1600 6400 1600
Wire Wire Line
	6800 1900 6400 1900
Wire Wire Line
	4500 1600 4800 1600
Wire Wire Line
	4800 1900 4500 1900
Wire Wire Line
	2900 1600 2600 1600
Wire Wire Line
	2600 1900 2900 1900
Wire Wire Line
	4500 2100 5650 2100
Wire Wire Line
	5650 2100 5650 2450
Wire Wire Line
	4500 2400 5250 2400
Wire Wire Line
	5250 2400 5250 2450
Wire Wire Line
	8400 2400 9100 2400
Wire Wire Line
	9100 2400 9100 2450
Wire Wire Line
	8400 2100 9700 2100
Wire Wire Line
	9700 2100 9700 2450
Wire Wire Line
	8400 1900 8700 1900
Wire Wire Line
	8400 1600 8700 1600
$Comp
L power:GND #PWR01
U 1 1 60A8063C
P 2350 1400
F 0 "#PWR01" H 2350 1150 50  0001 C CNN
F 1 "GND" H 2355 1227 50  0000 C CNN
F 2 "" H 2350 1400 50  0001 C CNN
F 3 "" H 2350 1400 50  0001 C CNN
	1    2350 1400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 60A8102F
P 6150 1400
F 0 "#PWR05" H 6150 1150 50  0001 C CNN
F 1 "GND" H 6155 1227 50  0000 C CNN
F 2 "" H 6150 1400 50  0001 C CNN
F 3 "" H 6150 1400 50  0001 C CNN
	1    6150 1400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 60A81A80
P 9100 2850
F 0 "#PWR07" H 9100 2600 50  0001 C CNN
F 1 "GND" H 9105 2677 50  0000 C CNN
F 2 "" H 9100 2850 50  0001 C CNN
F 3 "" H 9100 2850 50  0001 C CNN
	1    9100 2850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 60A81D59
P 9700 2850
F 0 "#PWR08" H 9700 2600 50  0001 C CNN
F 1 "GND" H 9705 2677 50  0000 C CNN
F 2 "" H 9700 2850 50  0001 C CNN
F 3 "" H 9700 2850 50  0001 C CNN
	1    9700 2850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 60A8203E
P 5650 2850
F 0 "#PWR04" H 5650 2600 50  0001 C CNN
F 1 "GND" H 5655 2677 50  0000 C CNN
F 2 "" H 5650 2850 50  0001 C CNN
F 3 "" H 5650 2850 50  0001 C CNN
	1    5650 2850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 60A8232F
P 5250 2850
F 0 "#PWR03" H 5250 2600 50  0001 C CNN
F 1 "GND" H 5255 2677 50  0000 C CNN
F 2 "" H 5250 2850 50  0001 C CNN
F 3 "" H 5250 2850 50  0001 C CNN
	1    5250 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	6150 1300 6150 1400
Wire Wire Line
	9100 2750 9100 2850
Wire Wire Line
	9700 2750 9700 2850
Wire Wire Line
	5650 2750 5650 2850
Wire Wire Line
	5250 2750 5250 2850
Wire Wire Line
	2350 1300 2350 1400
Wire Wire Line
	2900 2150 2700 2150
Wire Wire Line
	2700 2150 2700 3650
Wire Wire Line
	6800 2150 6650 2150
Wire Wire Line
	6650 2150 6650 2800
$Comp
L power:GND #PWR06
U 1 1 60A85C4D
P 6650 3650
F 0 "#PWR06" H 6650 3400 50  0001 C CNN
F 1 "GND" H 6655 3477 50  0000 C CNN
F 2 "" H 6650 3650 50  0001 C CNN
F 3 "" H 6650 3650 50  0001 C CNN
	1    6650 3650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 60A85F51
P 2700 3650
F 0 "#PWR02" H 2700 3400 50  0001 C CNN
F 1 "GND" H 2705 3477 50  0000 C CNN
F 2 "" H 2700 3650 50  0001 C CNN
F 3 "" H 2700 3650 50  0001 C CNN
	1    2700 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	6800 2350 6550 2350
Wire Wire Line
	2900 2350 2800 2350
Text HLabel 6550 2350 0    50   Input ~ 0
RS232_ENABLE
Text HLabel 2550 2350 0    50   Input ~ 0
RS232_ENABLE
Text Notes 7050 6050 0    100  ~ 0
Note:\nThis entire page (RS-232 interface) except for J7 is \noptional and may be left unpopulated if not needed.\nJ5 (male DB25, DTE connector) may be \nomitted if connection to a modem is not needed.\nJ6 (female DB25, DCE connector) may be \nomitted if direct connection to a DTE\n(with the framer as clock source) is not needed.\nJ7 (40 pin header) may be omitted if connection \nto a DEC bulkhead panel is not needed; this\nconnector is shared between RS-232 and integral\nmodem features
Wire Wire Line
	2900 2600 2800 2600
Wire Wire Line
	2800 2600 2800 2350
Connection ~ 2800 2350
Wire Wire Line
	2800 2350 2550 2350
Wire Wire Line
	2900 2800 2550 2800
Wire Wire Line
	2900 3000 2550 3000
Text HLabel 2550 2800 0    50   Input ~ 0
RS232_TXDATA
Text HLabel 2550 3000 0    50   Output ~ 0
RS232_RXDATA
Wire Wire Line
	6800 2600 6550 2600
Wire Wire Line
	6800 3000 6550 3000
Wire Wire Line
	6800 3200 6550 3200
Text HLabel 6550 2600 0    50   Input ~ 0
RS232_CLKOUT
Text HLabel 6550 3000 0    50   Output ~ 0
RS232_RXCLK
Text HLabel 6550 3200 0    50   Output ~ 0
RS232_TXCLK
Wire Wire Line
	6800 2800 6650 2800
Connection ~ 6650 2800
Wire Wire Line
	6650 2800 6650 3650
Text GLabel 5950 800  0    50   UnSpc ~ 0
V3_3
NoConn ~ 4500 3200
NoConn ~ 2900 3200
Wire Wire Line
	4500 2600 4700 2600
Wire Wire Line
	4500 2800 4700 2800
Wire Wire Line
	4500 3000 4700 3000
Wire Wire Line
	8400 2600 8650 2600
Wire Wire Line
	8400 3000 8650 3000
Wire Wire Line
	8400 3200 8650 3200
Text Label 4700 2600 0    50   ~ 0
RS232_1
Text Label 4700 2800 0    50   ~ 0
TXD
Text Label 4700 3000 0    50   ~ 0
RXD
Text Label 8650 2600 0    50   ~ 0
CLKOUT
Text Label 8650 3000 0    50   ~ 0
RXCLK
Text Label 8650 3200 0    50   ~ 0
TXCLK
$Comp
L Connector:DB25_Male_MountingHoles J5
U 1 1 60B31BFA
P 2400 5650
F 0 "J5" H 2580 5652 50  0000 L CNN
F 1 "DTE" H 2580 5561 50  0000 L CNN
F 2 "ddcmp_parts:Amphenol_DB25M_PCB" H 2400 5650 50  0001 C CNN
F 3 "https://www.amphenol-icc.com/media/wysiwyg/files/drawing/l717sdxxxp1ach4f.pdf" H 2400 5650 50  0001 C CNN
F 4 "Amphenol ICC" H 2400 5650 50  0001 C CNN "Vendor"
F 5 "L717SDB25P1ACH4F-ND" H 2400 5650 50  0001 C CNN "Digikey part"
F 6 "L717SDB25P1ACH4F" H 2400 5650 50  0001 C CNN "Vendor part"
	1    2400 5650
	1    0    0    -1  
$EndComp
$Comp
L Connector:DB25_Female_MountingHoles J6
U 1 1 60B3290B
P 4400 5650
F 0 "J6" H 4580 5652 50  0000 L CNN
F 1 "DCE" H 4580 5561 50  0000 L CNN
F 2 "ddcmp_parts:Amphenol_DB25F_PCB" H 4400 5650 50  0001 C CNN
F 3 "https://www.amphenol-icc.com/media/wysiwyg/files/drawing/l77sdxxxs1ach3r.pdf" H 4400 5650 50  0001 C CNN
F 4 "L77SDB25S1ACH3R-ND" H 4400 5650 50  0001 C CNN "Digikey part"
F 5 "Amphenol ICC" H 4400 5650 50  0001 C CNN "Vendor"
F 6 "L77SDB25S1ACH3R" H 4400 5650 50  0001 C CNN "Vendor part"
	1    4400 5650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 60A731C7
P 2400 7200
F 0 "#PWR0105" H 2400 6950 50  0001 C CNN
F 1 "GND" H 2405 7027 50  0000 C CNN
F 2 "" H 2400 7200 50  0001 C CNN
F 3 "" H 2400 7200 50  0001 C CNN
	1    2400 7200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0114
U 1 1 60A73F97
P 4400 7200
F 0 "#PWR0114" H 4400 6950 50  0001 C CNN
F 1 "GND" H 4405 7027 50  0000 C CNN
F 2 "" H 4400 7200 50  0001 C CNN
F 3 "" H 4400 7200 50  0001 C CNN
	1    4400 7200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4400 7200 4400 7150
Wire Wire Line
	2400 7050 2400 7150
Wire Wire Line
	2100 6850 1950 6850
Wire Wire Line
	1950 6850 1950 7150
Wire Wire Line
	1950 7150 2400 7150
Connection ~ 2400 7150
Wire Wire Line
	2400 7150 2400 7200
Wire Wire Line
	4100 4450 3850 4450
Wire Wire Line
	3850 7150 4400 7150
Connection ~ 4400 7150
Wire Wire Line
	4400 7150 4400 7050
Wire Wire Line
	3850 4450 3850 5650
Wire Wire Line
	4100 5650 3850 5650
Connection ~ 3850 5650
Wire Wire Line
	3850 5650 3850 7150
Wire Wire Line
	2100 5650 1950 5650
Wire Wire Line
	1950 5650 1950 6850
Connection ~ 1950 6850
Wire Wire Line
	8400 2800 8650 2800
Text Label 8650 2800 0    50   ~ 0
RS232_0
$Comp
L ddcmp_symbols:Berg_40_pin_DEC_labeling J?
U 1 1 60AB6B87
P 6000 5650
AR Path="/60A471E8/60AB6B87" Ref="J?"  Part="1" 
AR Path="/60A47002/60AB6B87" Ref="J7"  Part="1" 
F 0 "J7" H 6050 6767 50  0000 C CNN
F 1 "Bulkhead" H 6050 6676 50  0000 C CNN
F 2 "ddcmp_parts:Berg_40_pin_DEC_pinout" H 6000 5650 50  0001 C CNN
F 3 "https://cdn.amphenol-icc.com/media/wysiwyg/files/drawing/75869.pdf" H 6000 5650 50  0001 C CNN
F 4 "609-1779-ND" H 0   0   50  0001 C CNN "Digikey part"
F 5 "Amphenol ICC" H 0   0   50  0001 C CNN "Vendor"
F 6 "75869-107LF" H 0   0   50  0001 C CNN "Vendor part"
	1    6000 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 4750 5650 4750
Wire Wire Line
	5650 4750 5650 6650
$Comp
L power:GND #PWR?
U 1 1 60AB6B8F
P 5650 6850
AR Path="/60A471E8/60AB6B8F" Ref="#PWR?"  Part="1" 
AR Path="/60A47002/60AB6B8F" Ref="#PWR09"  Part="1" 
F 0 "#PWR09" H 5650 6600 50  0001 C CNN
F 1 "GND" H 5655 6677 50  0000 C CNN
F 2 "" H 5650 6850 50  0001 C CNN
F 3 "" H 5650 6850 50  0001 C CNN
	1    5650 6850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 60AB6B95
P 6450 6800
AR Path="/60A471E8/60AB6B95" Ref="#PWR?"  Part="1" 
AR Path="/60A47002/60AB6B95" Ref="#PWR010"  Part="1" 
F 0 "#PWR010" H 6450 6550 50  0001 C CNN
F 1 "GND" H 6455 6627 50  0000 C CNN
F 2 "" H 6450 6800 50  0001 C CNN
F 3 "" H 6450 6800 50  0001 C CNN
	1    6450 6800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 6800 6450 6650
Wire Wire Line
	6450 6650 6300 6650
Wire Wire Line
	6450 6650 6450 4750
Wire Wire Line
	6450 4750 6300 4750
Connection ~ 6450 6650
Wire Wire Line
	5800 5850 5550 5850
Text Label 5450 5850 2    50   ~ 0
RS232_1
Wire Wire Line
	5800 5450 5550 5450
Wire Wire Line
	5550 5450 5550 5850
Connection ~ 5550 5850
Wire Wire Line
	5550 5850 5450 5850
Wire Wire Line
	5800 6350 5450 6350
Wire Wire Line
	5800 6050 5450 6050
Text Label 5450 6350 2    50   ~ 0
RXD
Text Label 5450 6050 2    50   ~ 0
RXCLK
Wire Wire Line
	5800 6150 5450 6150
Text Label 5450 6150 2    50   ~ 0
TXCLK
Wire Wire Line
	5800 6450 5450 6450
Text Label 5450 6450 2    50   ~ 0
TXD
Wire Wire Line
	5800 5350 5450 5350
Wire Wire Line
	5800 5250 5450 5250
Wire Wire Line
	5800 5050 5450 5050
Wire Wire Line
	6300 5050 6600 5050
Text HLabel 6600 5050 2    50   Output ~ 0
IM_RXD_N
Text HLabel 5450 5050 0    50   Output ~ 0
IM_RDX
Text HLabel 5450 5250 0    50   Input ~ 0
IM_TXD
Text HLabel 5450 5350 0    50   Input ~ 0
IM_TXD_N
Wire Wire Line
	2100 5550 1850 5550
Wire Wire Line
	4100 5850 3750 5850
Text Label 1700 5550 2    50   ~ 0
RS232_1
Text Label 3650 5850 2    50   ~ 0
RS232_1
Wire Wire Line
	4100 5450 3750 5450
Wire Wire Line
	3750 5450 3750 5850
Connection ~ 3750 5850
Wire Wire Line
	3750 5850 3650 5850
Wire Wire Line
	4100 6150 3650 6150
Text Label 3650 6150 2    50   ~ 0
RS232_0
Wire Wire Line
	2100 6250 1850 6250
Wire Wire Line
	1850 6250 1850 5550
Connection ~ 1850 5550
Wire Wire Line
	1850 5550 1700 5550
Wire Wire Line
	4100 5250 3750 5250
Wire Wire Line
	3750 5250 3750 5450
Connection ~ 3750 5450
Wire Wire Line
	2100 6650 1700 6650
Text Label 1700 6650 2    50   ~ 0
TXD
Wire Wire Line
	2100 6450 1700 6450
Text Label 1700 6450 2    50   ~ 0
RXD
Wire Wire Line
	4100 4650 3650 4650
Text Label 3650 4650 2    50   ~ 0
RXD
Wire Wire Line
	4100 4850 3650 4850
Text Label 3650 4850 2    50   ~ 0
TXD
Wire Wire Line
	2100 6550 1700 6550
Wire Wire Line
	2100 6150 1700 6150
Wire Wire Line
	4100 5150 3650 5150
Wire Wire Line
	4100 4750 3650 4750
NoConn ~ 4100 4550
NoConn ~ 4100 4950
NoConn ~ 4100 5050
NoConn ~ 4100 5350
NoConn ~ 4100 5550
NoConn ~ 4100 5750
NoConn ~ 4100 5950
NoConn ~ 4100 6050
NoConn ~ 4100 6250
NoConn ~ 4100 6350
NoConn ~ 4100 6450
NoConn ~ 4100 6550
NoConn ~ 4100 6650
NoConn ~ 4100 6750
NoConn ~ 4100 6850
NoConn ~ 2100 4450
NoConn ~ 2100 4550
NoConn ~ 2100 4650
NoConn ~ 2100 4750
NoConn ~ 2100 4850
NoConn ~ 2100 4950
NoConn ~ 2100 5050
NoConn ~ 2100 5150
NoConn ~ 2100 5250
NoConn ~ 2100 5350
NoConn ~ 2100 5450
NoConn ~ 2100 5750
NoConn ~ 2100 5850
NoConn ~ 2100 5950
NoConn ~ 2100 6050
NoConn ~ 2100 6350
NoConn ~ 2100 6750
NoConn ~ 5800 4850
NoConn ~ 5800 4950
NoConn ~ 5800 5150
NoConn ~ 5800 5550
NoConn ~ 5800 5650
NoConn ~ 5800 5750
NoConn ~ 5800 5950
NoConn ~ 5800 6250
NoConn ~ 5800 6550
NoConn ~ 6300 6550
NoConn ~ 6300 6450
NoConn ~ 6300 6350
NoConn ~ 6300 6250
NoConn ~ 6300 6150
NoConn ~ 6300 6050
NoConn ~ 6300 5950
NoConn ~ 6300 5850
NoConn ~ 6300 5750
NoConn ~ 6300 5650
NoConn ~ 6300 5550
NoConn ~ 6300 5450
NoConn ~ 6300 5350
NoConn ~ 6300 5250
NoConn ~ 6300 5150
NoConn ~ 6300 4950
NoConn ~ 6300 4850
Wire Wire Line
	5800 6650 5650 6650
Connection ~ 5650 6650
Wire Wire Line
	5650 6650 5650 6850
Text Label 1700 6550 2    50   ~ 0
TXCLK
Text Label 1700 6150 2    50   ~ 0
RXCLK
Text Label 3650 4750 2    50   ~ 0
CLKOUT
Text Label 3650 5150 2    50   ~ 0
CLKOUT
Text Notes 1700 6250 0    50   ~ 0
RTS
Text Notes 1900 5550 0    50   ~ 0
DTR
Text Notes 5500 5450 0    50   ~ 0
DTR
Text Notes 5700 5850 0    50   ~ 0
RTS
Text Notes 3900 5850 0    50   ~ 0
CD
Text Notes 3900 5450 0    50   ~ 0
DSR
Text Notes 3900 5250 0    50   ~ 0
CTS
Text Notes 3900 6150 0    50   ~ 0
RI
$EndSCHEMATC
