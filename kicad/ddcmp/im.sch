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
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	4550 3100 4350 3100
Connection ~ 4350 3100
Wire Wire Line
	4350 2900 4350 3100
Wire Wire Line
	4550 2900 4350 2900
Wire Wire Line
	4350 3100 4350 3800
Wire Wire Line
	4350 3800 4350 4150
Connection ~ 4350 3800
Wire Wire Line
	4550 3800 4350 3800
$Comp
L power:GND #PWR?
U 1 1 60A50952
P 4350 4700
AR Path="/60A50952" Ref="#PWR?"  Part="1" 
AR Path="/60A471E8/60A50952" Ref="#PWR0104"  Part="1" 
F 0 "#PWR0104" H 4350 4450 50  0001 C CNN
F 1 "GND" H 4355 4527 50  0000 C CNN
F 2 "" H 4350 4700 50  0001 C CNN
F 3 "" H 4350 4700 50  0001 C CNN
	1    4350 4700
	1    0    0    -1  
$EndComp
Connection ~ 4350 2900
Wire Wire Line
	4350 2200 4350 2800
Wire Wire Line
	4550 2200 4350 2200
Wire Wire Line
	6400 3300 6150 3300
Wire Wire Line
	6400 3450 6400 3300
Wire Wire Line
	6750 3450 6400 3450
Wire Wire Line
	6400 3050 6750 3050
Wire Wire Line
	6400 3200 6400 3050
Wire Wire Line
	6150 3200 6400 3200
$Comp
L ddcmp_symbols:max3089ecpd U?
U 1 1 60A50967
P 4550 2200
AR Path="/60A50967" Ref="U?"  Part="1" 
AR Path="/60A471E8/60A50967" Ref="U2"  Part="1" 
F 0 "U2" H 5350 2587 60  0000 C CNN
F 1 "max3089ecpd" H 5350 2481 60  0000 C CNN
F 2 "ddcmp_parts:DIP-14_W7.62mm" H 5350 2440 60  0001 C CNN
F 3 "" H 4550 2200 60  0000 C CNN
	1    4550 2200
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:ADT1-6T TR?
U 1 1 60A5097C
P 3300 3100
AR Path="/60A5097C" Ref="TR?"  Part="1" 
AR Path="/60A471E8/60A5097C" Ref="TR1"  Part="1" 
F 0 "TR1" H 3300 2675 50  0000 C CNN
F 1 "ADT1-6T" H 3300 2766 50  0000 C CNN
F 2 "ddcmp_parts:Mini-Circuits_CD637_H5.23mm" H 3300 2750 50  0001 C CNN
F 3 "https://www.minicircuits.com/pdfs/ADT1-6T+.pdf" H 3300 3100 50  0001 C CNN
	1    3300 3100
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:ADT1-6T TR?
U 1 1 60A50982
P 6950 3250
AR Path="/60A50982" Ref="TR?"  Part="1" 
AR Path="/60A471E8/60A50982" Ref="TR2"  Part="1" 
F 0 "TR2" H 6950 2825 50  0000 C CNN
F 1 "ADT1-6T" H 6950 2916 50  0000 C CNN
F 2 "ddcmp_parts:Mini-Circuits_CD637_H5.23mm" H 6950 2900 50  0001 C CNN
F 3 "https://www.minicircuits.com/pdfs/ADT1-6T+.pdf" H 6950 3250 50  0001 C CNN
	1    6950 3250
	-1   0    0    1   
$EndComp
Wire Wire Line
	4550 3600 4100 3600
Wire Wire Line
	4350 2800 4550 2800
Connection ~ 4350 2800
Wire Wire Line
	4350 2800 4350 2900
Wire Wire Line
	6150 2300 6350 2300
Text HLabel 6350 2300 2    50   Output ~ 0
IM_RXDATA
Wire Wire Line
	4550 3000 4100 3000
Wire Wire Line
	4550 2600 4100 2600
Wire Wire Line
	4550 2500 4100 2500
Text HLabel 4100 2500 0    50   Input ~ 0
IM_ENABLE
Text HLabel 4100 2600 0    50   Input ~ 0
IM_TXDATA
Text HLabel 4100 3000 0    50   3State ~ 0
IM_SRL
Wire Wire Line
	3500 2900 3550 2900
Wire Wire Line
	3550 2900 3550 2300
Wire Wire Line
	3550 2300 4550 2300
Wire Wire Line
	3500 3300 3650 3300
Wire Wire Line
	3650 3300 3650 2700
Wire Wire Line
	3650 2700 4450 2700
Wire Wire Line
	4450 2700 4450 2400
Wire Wire Line
	4450 2400 4550 2400
$Comp
L ddcmp_symbols:Conn_Coaxial J3
U 1 1 60A8F3C5
P 8700 3050
F 0 "J3" H 8800 3025 50  0000 L CNN
F 1 "TX" H 8800 2934 50  0000 L CNN
F 2 "ddcmp_parts:BNC_Molex_J_RA_PCB" H 8700 3050 50  0001 C CNN
F 3 " ~" H 8700 3050 50  0001 C CNN
	1    8700 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	8500 3050 7150 3050
Wire Wire Line
	7150 3450 7300 3450
Wire Wire Line
	8700 3450 8700 3250
$Comp
L ddcmp_symbols:R_US R2
U 1 1 60A91D4D
P 7300 3700
F 0 "R2" H 7368 3746 50  0000 L CNN
F 1 "1M" H 7368 3655 50  0000 L CNN
F 2 "ddcmp_parts:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 7340 3690 50  0001 C CNN
F 3 "" H 7300 3700 50  0001 C CNN
	1    7300 3700
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:R_US R1
U 1 1 60A93074
P 2850 3600
F 0 "R1" H 2918 3646 50  0000 L CNN
F 1 "1M" H 2918 3555 50  0000 L CNN
F 2 "ddcmp_parts:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 2890 3590 50  0001 C CNN
F 3 "" H 2850 3600 50  0001 C CNN
	1    2850 3600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 60A93772
P 7300 3950
F 0 "#PWR0109" H 7300 3700 50  0001 C CNN
F 1 "GND" H 7305 3777 50  0000 C CNN
F 2 "" H 7300 3950 50  0001 C CNN
F 3 "" H 7300 3950 50  0001 C CNN
	1    7300 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	7300 3550 7300 3450
Connection ~ 7300 3450
Wire Wire Line
	7300 3450 8700 3450
Wire Wire Line
	7300 3950 7300 3850
Wire Wire Line
	2850 3750 2850 3850
$Comp
L power:GND #PWR0110
U 1 1 60A962E8
P 2850 3850
F 0 "#PWR0110" H 2850 3600 50  0001 C CNN
F 1 "GND" H 2855 3677 50  0000 C CNN
F 2 "" H 2850 3850 50  0001 C CNN
F 3 "" H 2850 3850 50  0001 C CNN
	1    2850 3850
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:Conn_Coaxial J2
U 1 1 60A974EB
P 1550 2900
F 0 "J2" H 1478 3138 50  0000 C CNN
F 1 "RX" H 1478 3047 50  0000 C CNN
F 2 "ddcmp_parts:BNC_Molex_J_RA_PCB" H 1550 2900 50  0001 C CNN
F 3 " ~" H 1550 2900 50  0001 C CNN
	1    1550 2900
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1750 2900 3100 2900
Wire Wire Line
	3100 3300 2850 3300
Wire Wire Line
	1550 3300 1550 3100
Wire Wire Line
	2850 3450 2850 3300
Connection ~ 2850 3300
Wire Wire Line
	2850 3300 1550 3300
$Comp
L ddcmp_symbols:C C11
U 1 1 60AAD020
P 4100 3900
F 0 "C11" H 4215 3946 50  0000 L CNN
F 1 "0.1uF" H 4215 3855 50  0000 L CNN
F 2 "ddcmp_parts:C_Rect_L4.0mm_W2.5mm_P2.54mm" H 4138 3750 50  0001 C CNN
F 3 "" H 4100 3900 50  0001 C CNN
F 4 "399-14010-1-ND​" H 4100 3900 50  0001 C CNN "Digikey"
F 5 "Kemet" H 4100 3900 50  0001 C CNN "Mfg"
F 6 "C315C104M5U5TA7301" H 4100 3900 50  0001 C CNN "Mfg part"
	1    4100 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 3750 4100 3600
Connection ~ 4100 3600
Wire Wire Line
	4100 3600 3950 3600
Wire Wire Line
	4100 4050 4100 4150
Wire Wire Line
	4100 4150 4350 4150
Connection ~ 4350 4150
Wire Wire Line
	4350 4150 4350 4700
Text Notes 6000 6250 0    100  ~ 0
Note:\nThis entire page (the integral modem feature) is optional and\nmay be left unpopulated if the feature is not needed.
NoConn ~ 6750 3250
NoConn ~ 3500 3100
Text GLabel 3950 3600 0    50   UnSpc ~ 0
V3_3
$EndSCHEMATC
