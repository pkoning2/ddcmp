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
Comment1 ""
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
F 3 "" H 7600 2600 50  0001 C CNN
F 4 "Maxim" H 7600 2500 50  0001 C CNN "Mfg"
F 5 "MAX3222ECPN" H 7600 2500 50  0001 C CNN "Mfg part"
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
F 3 "" H 3700 2600 50  0001 C CNN
F 4 "Maxim" H 3700 2500 50  0001 C CNN "Mfg"
F 5 "MAX3222ECPN" H 3700 2500 50  0001 C CNN "Mfg part"
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
F 4 "399-14010-1-ND​" H 2600 1750 50  0001 C CNN "Digikey"
F 5 "Kemet" H 2600 1750 50  0001 C CNN "Mfg"
F 6 "C315C104M5U5TA7301" H 2600 1750 50  0001 C CNN "Mfg part"
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
F 4 "399-14010-1-ND​" H 4800 1750 50  0001 C CNN "Digikey"
F 5 "Kemet" H 4800 1750 50  0001 C CNN "Mfg"
F 6 "C315C104M5U5TA7301" H 4800 1750 50  0001 C CNN "Mfg part"
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
F 4 "399-14010-1-ND​" H 6400 1750 50  0001 C CNN "Digikey"
F 5 "Kemet" H 6400 1750 50  0001 C CNN "Mfg"
F 6 "C315C104M5U5TA7301" H 6400 1750 50  0001 C CNN "Mfg part"
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
F 4 "399-14010-1-ND​" H 8700 1750 50  0001 C CNN "Digikey"
F 5 "Kemet" H 8700 1750 50  0001 C CNN "Mfg"
F 6 "C315C104M5U5TA7301" H 8700 1750 50  0001 C CNN "Mfg part"
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
F 4 "399-14010-1-ND​" H 2350 1150 50  0001 C CNN "Digikey"
F 5 "Kemet" H 2350 1150 50  0001 C CNN "Mfg"
F 6 "C315C104M5U5TA7301" H 2350 1150 50  0001 C CNN "Mfg part"
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
F 4 "399-14010-1-ND​" H 6150 1150 50  0001 C CNN "Digikey"
F 5 "Kemet" H 6150 1150 50  0001 C CNN "Mfg"
F 6 "C315C104M5U5TA7301" H 6150 1150 50  0001 C CNN "Mfg part"
	1    6150 1150
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:C C5
U 1 1 60A798E1
P 5550 2600
F 0 "C5" H 5665 2646 50  0000 L CNN
F 1 "0.1uF" H 5665 2555 50  0000 L CNN
F 2 "ddcmp_parts:C_Rect_L4.0mm_W2.5mm_P2.54mm" H 5588 2450 50  0001 C CNN
F 3 "" H 5550 2600 50  0001 C CNN
F 4 "399-14010-1-ND​" H 5550 2600 50  0001 C CNN "Digikey"
F 5 "Kemet" H 5550 2600 50  0001 C CNN "Mfg"
F 6 "C315C104M5U5TA7301" H 5550 2600 50  0001 C CNN "Mfg part"
	1    5550 2600
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:C C4
U 1 1 60A7AB61
P 5150 2600
F 0 "C4" H 5265 2646 50  0000 L CNN
F 1 "0.1uF" H 5265 2555 50  0000 L CNN
F 2 "ddcmp_parts:C_Rect_L4.0mm_W2.5mm_P2.54mm" H 5188 2450 50  0001 C CNN
F 3 "" H 5150 2600 50  0001 C CNN
F 4 "399-14010-1-ND​" H 5150 2600 50  0001 C CNN "Digikey"
F 5 "Kemet" H 5150 2600 50  0001 C CNN "Mfg"
F 6 "C315C104M5U5TA7301" H 5150 2600 50  0001 C CNN "Mfg part"
	1    5150 2600
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
F 4 "399-14010-1-ND​" H 9100 2600 50  0001 C CNN "Digikey"
F 5 "Kemet" H 9100 2600 50  0001 C CNN "Mfg"
F 6 "C315C104M5U5TA7301" H 9100 2600 50  0001 C CNN "Mfg part"
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
F 4 "399-14010-1-ND​" H 9700 2600 50  0001 C CNN "Digikey"
F 5 "Kemet" H 9700 2600 50  0001 C CNN "Mfg"
F 6 "C315C104M5U5TA7301" H 9700 2600 50  0001 C CNN "Mfg part"
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
	4500 2100 5550 2100
Wire Wire Line
	5550 2100 5550 2450
Wire Wire Line
	4500 2400 5150 2400
Wire Wire Line
	5150 2400 5150 2450
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
P 5550 2850
F 0 "#PWR04" H 5550 2600 50  0001 C CNN
F 1 "GND" H 5555 2677 50  0000 C CNN
F 2 "" H 5550 2850 50  0001 C CNN
F 3 "" H 5550 2850 50  0001 C CNN
	1    5550 2850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 60A8232F
P 5150 2850
F 0 "#PWR03" H 5150 2600 50  0001 C CNN
F 1 "GND" H 5155 2677 50  0000 C CNN
F 2 "" H 5150 2850 50  0001 C CNN
F 3 "" H 5150 2850 50  0001 C CNN
	1    5150 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	6150 1300 6150 1400
Wire Wire Line
	9100 2750 9100 2850
Wire Wire Line
	9700 2750 9700 2850
Wire Wire Line
	5550 2750 5550 2850
Wire Wire Line
	5150 2750 5150 2850
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
RS232_CLKEN
Text HLabel 2550 2350 0    50   Input ~ 0
RS232_ENABLE
Text Notes 7000 6050 0    100  ~ 0
Note:\nThis entire page (RS-232 interface) is \noptional and may be left unpopulated if not needed.\nJ_n (male DB25, DTE connector) may be \nomitted if connection to a modem is not needed.\nJ_m (female DB25, DCE connector) may be \nomitted if direct connection to a DTE\n(with the framer as clock source) is not needed.\nJ_x (40 pin header) may be omitted if connection \nto a DEC bulkhead panel is not needed.
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
NoConn ~ 8400 2800
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
RTS
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
P 2350 5950
F 0 "J5" H 2530 5952 50  0000 L CNN
F 1 "DTE" H 2530 5861 50  0000 L CNN
F 2 "" H 2350 5950 50  0001 C CNN
F 3 " ~" H 2350 5950 50  0001 C CNN
	1    2350 5950
	1    0    0    -1  
$EndComp
$Comp
L Connector:DB25_Female_MountingHoles J6
U 1 1 60B3290B
P 4750 6000
F 0 "J6" H 4930 6002 50  0000 L CNN
F 1 "DCE" H 4930 5911 50  0000 L CNN
F 2 "" H 4750 6000 50  0001 C CNN
F 3 " ~" H 4750 6000 50  0001 C CNN
	1    4750 6000
	1    0    0    -1  
$EndComp
$EndSCHEMATC
