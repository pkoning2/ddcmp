EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 4
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
	4550 3250 4450 3250
Connection ~ 4450 3250
Wire Wire Line
	4450 3250 4450 3350
Wire Wire Line
	4450 3350 4450 4150
Connection ~ 4450 3350
Wire Wire Line
	4550 3350 4450 3350
$Comp
L power:GND #PWR?
U 1 1 60A50952
P 4450 4300
AR Path="/60A50952" Ref="#PWR?"  Part="1" 
AR Path="/60A471E8/60A50952" Ref="#PWR0104"  Part="1" 
F 0 "#PWR0104" H 4450 4050 50  0001 C CNN
F 1 "GND" H 4455 4127 50  0000 C CNN
F 2 "" H 4450 4300 50  0001 C CNN
F 3 "" H 4450 4300 50  0001 C CNN
	1    4450 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 2200 4450 2200
Wire Wire Line
	6400 2850 6150 2850
Wire Wire Line
	6400 3000 6400 2850
Wire Wire Line
	6750 3000 6550 3000
Wire Wire Line
	6400 2600 6550 2600
Wire Wire Line
	6400 2750 6400 2600
Wire Wire Line
	6150 2750 6400 2750
$Comp
L ddcmp_symbols:max3491ecpd U?
U 1 1 60A50967
P 4550 2200
AR Path="/60A50967" Ref="U?"  Part="1" 
AR Path="/60A471E8/60A50967" Ref="U2"  Part="1" 
F 0 "U2" H 5350 2587 60  0000 C CNN
F 1 "max3491ecpd" H 5350 2481 60  0000 C CNN
F 2 "ddcmp_parts:DIP-14_W7.62mm" H 5350 2440 60  0001 C CNN
F 3 "https://datasheets.maximintegrated.com/en/ds/MAX3483-MAX3491.pdf" H 4550 2200 60  0001 C CNN
F 4 "MAX3491CPD+-ND" H 4550 2200 50  0001 C CNN "Digikey part"
F 5 "Maxim" H 4550 2200 50  0001 C CNN "Vendor"
F 6 "MAX3491CPD+" H 4550 2200 50  0001 C CNN "Vendor part"
	1    4550 2200
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:TT1-6-X65+ TR?
U 1 1 60A5097C
P 2800 3100
AR Path="/60A5097C" Ref="TR?"  Part="1" 
AR Path="/60A471E8/60A5097C" Ref="TR1"  Part="1" 
F 0 "TR1" H 2800 2675 50  0000 C CNN
F 1 "TT1-6-X65+" H 2800 2766 50  0000 C CNN
F 2 "ddcmp_parts:Mini-Circuits_CD637_X65" H 2800 2750 50  0001 C CNN
F 3 "https://www.minicircuits.com/pdfs/TT1-6-X65.pdf" H 2800 3100 50  0001 C CNN
F 4 "" H 0   0   50  0001 C CNN "Digikey part"
F 5 "Mini-Circuits" H 0   0   50  0001 C CNN "Vendor"
F 6 "TT1-6-X65+" H 0   0   50  0001 C CNN "Vendor part"
	1    2800 3100
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:TT1-6-X65+ TR?
U 1 1 60A50982
P 6950 2800
AR Path="/60A50982" Ref="TR?"  Part="1" 
AR Path="/60A471E8/60A50982" Ref="TR2"  Part="1" 
F 0 "TR2" H 6950 2375 50  0000 C CNN
F 1 "TT1-6-X65+" H 6950 2466 50  0000 C CNN
F 2 "ddcmp_parts:Mini-Circuits_CD637_X65" H 6950 2450 50  0001 C CNN
F 3 "https://www.minicircuits.com/pdfs/TT1-6-X65.pdf" H 6950 2800 50  0001 C CNN
F 4 "" H 0   0   50  0001 C CNN "Digikey part"
F 5 "Mini-Circuits" H 0   0   50  0001 C CNN "Vendor"
F 6 "TT1-6-X65+" H 0   0   50  0001 C CNN "Vendor part"
	1    6950 2800
	-1   0    0    1   
$EndComp
Wire Wire Line
	6150 2300 6350 2300
Text HLabel 6350 2300 2    50   Output ~ 0
IM_RXDATA
Wire Wire Line
	4550 2850 4000 2850
Wire Wire Line
	4550 2750 4000 2750
Text HLabel 4000 2750 0    50   Input ~ 0
IM_ENABLE
Text HLabel 4000 2850 0    50   Input ~ 0
IM_TXDATA
Wire Wire Line
	3000 2900 3100 2900
Wire Wire Line
	3200 2900 3200 2300
Wire Wire Line
	3000 3300 3100 3300
Wire Wire Line
	3300 3300 3300 2400
$Comp
L ddcmp_symbols:Conn_Coaxial J3
U 1 1 60A8F3C5
P 8700 2600
F 0 "J3" H 8800 2575 50  0000 L CNN
F 1 "TX" H 8800 2484 50  0000 L CNN
F 2 "ddcmp_parts:BNC_Molex_J_RA_PCB" H 8700 2600 50  0001 C CNN
F 3 "https://www.molex.com/pdm_docs/sd/731000105_sd.pdf" H 8700 2600 50  0001 C CNN
F 4 "WM20430-ND" H 0   0   50  0001 C CNN "Digikey part"
F 5 "Molex" H 0   0   50  0001 C CNN "Vendor"
F 6 "0731713150" H 0   0   50  0001 C CNN "Vendor part"
	1    8700 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	8500 2600 7850 2600
Wire Wire Line
	7150 3000 7300 3000
Wire Wire Line
	8700 3000 8700 2800
$Comp
L ddcmp_symbols:R_US R2
U 1 1 60A91D4D
P 7300 3250
F 0 "R2" H 7368 3296 50  0000 L CNN
F 1 "1M" H 7368 3205 50  0000 L CNN
F 2 "ddcmp_parts:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 7340 3240 50  0001 C CNN
F 3 "" H 7300 3250 50  0001 C CNN
F 4 "1.0MQBK-ND" H 7300 3250 50  0001 C CNN "Digikey part"
F 5 "Yageo" H 7300 3250 50  0001 C CNN "Vendor"
F 6 "CFR-25JB-52-1M" H 7300 3250 50  0001 C CNN "Vendor part"
	1    7300 3250
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:R_US R1
U 1 1 60A93074
P 2500 3600
F 0 "R1" H 2568 3646 50  0000 L CNN
F 1 "1M" H 2568 3555 50  0000 L CNN
F 2 "ddcmp_parts:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 2540 3590 50  0001 C CNN
F 3 "" H 2500 3600 50  0001 C CNN
F 4 "1.0MQBK-ND" H 2500 3600 50  0001 C CNN "Digikey part"
F 5 "Yageo" H 2500 3600 50  0001 C CNN "Vendor"
F 6 "CFR-25JB-52-1M" H 2500 3600 50  0001 C CNN "Vendor part"
	1    2500 3600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 60A93772
P 7300 3500
F 0 "#PWR0109" H 7300 3250 50  0001 C CNN
F 1 "GND" H 7305 3327 50  0000 C CNN
F 2 "" H 7300 3500 50  0001 C CNN
F 3 "" H 7300 3500 50  0001 C CNN
	1    7300 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	7300 3100 7300 3000
Connection ~ 7300 3000
Wire Wire Line
	7300 3000 7850 3000
Wire Wire Line
	7300 3500 7300 3400
Wire Wire Line
	2500 3750 2500 3850
$Comp
L power:GND #PWR0110
U 1 1 60A962E8
P 2500 3850
F 0 "#PWR0110" H 2500 3600 50  0001 C CNN
F 1 "GND" H 2505 3677 50  0000 C CNN
F 2 "" H 2500 3850 50  0001 C CNN
F 3 "" H 2500 3850 50  0001 C CNN
	1    2500 3850
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:Conn_Coaxial J2
U 1 1 60A974EB
P 1200 2900
F 0 "J2" H 1128 3138 50  0000 C CNN
F 1 "RX" H 1128 3047 50  0000 C CNN
F 2 "ddcmp_parts:BNC_Molex_J_RA_PCB" H 1200 2900 50  0001 C CNN
F 3 "https://www.molex.com/pdm_docs/sd/731000105_sd.pdf" H 1200 2900 50  0001 C CNN
F 4 "WM20430-ND" H 0   0   50  0001 C CNN "Digikey part"
F 5 "Molex" H 0   0   50  0001 C CNN "Vendor"
F 6 "0731713150" H 0   0   50  0001 C CNN "Vendor part"
	1    1200 2900
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1400 2900 2100 2900
Wire Wire Line
	2600 3300 2500 3300
Wire Wire Line
	1200 3300 1200 3100
Wire Wire Line
	2500 3450 2500 3300
Connection ~ 2500 3300
Wire Wire Line
	2500 3300 2100 3300
$Comp
L ddcmp_symbols:C C11
U 1 1 60AAD020
P 4100 3750
F 0 "C11" H 4215 3796 50  0000 L CNN
F 1 "0.1uF" H 4215 3705 50  0000 L CNN
F 2 "ddcmp_parts:C_Rect_L4.0mm_W2.5mm_P2.54mm" H 4138 3600 50  0001 C CNN
F 3 "" H 4100 3750 50  0001 C CNN
F 4 "399-14010-1-ND" H 4100 3750 50  0001 C CNN "Digikey part"
F 5 "Kemet" H 4100 3750 50  0001 C CNN "Vendor"
F 6 "C315C104M5U5TA7301" H 4100 3750 50  0001 C CNN "Vendor part"
F 7 "399-14010-1-ND" H 4100 3750 50  0001 C CNN "Digikey part"
F 8 "Kemet" H 4100 3750 50  0001 C CNN "Vendor"
F 9 "C315C104M5U5TA7301" H 4100 3750 50  0001 C CNN "Vendor part"
	1    4100 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 3900 4100 4150
Wire Wire Line
	4100 4150 4450 4150
Connection ~ 4450 4150
Text Notes 6000 6250 0    100  ~ 0
Note:\nThis entire page (the integral modem feature) is optional and\nmay be left unpopulated if the feature is not needed.
NoConn ~ 6750 2800
Text GLabel 2800 1200 0    50   UnSpc ~ 0
V3_3
Wire Wire Line
	7850 2600 7850 2400
Wire Wire Line
	7850 2400 8000 2400
Connection ~ 7850 2600
Wire Wire Line
	7850 2600 7150 2600
Wire Wire Line
	7850 3000 7850 3200
Wire Wire Line
	7850 3200 8000 3200
Connection ~ 7850 3000
Wire Wire Line
	7850 3000 8700 3000
Text HLabel 8000 3200 2    50   Output ~ 0
IM_TXD_N
Text HLabel 8000 2400 2    50   Output ~ 0
IM_TDX
Wire Wire Line
	2100 2900 2100 2650
Wire Wire Line
	2100 2650 1900 2650
Connection ~ 2100 2900
Wire Wire Line
	2100 2900 2600 2900
Wire Wire Line
	2100 3300 2100 3550
Wire Wire Line
	2100 3550 1900 3550
Connection ~ 2100 3300
Wire Wire Line
	2100 3300 1200 3300
Text HLabel 1900 3550 0    50   Input ~ 0
IM_RXD_N
Text HLabel 1900 2650 0    50   Input ~ 0
IM_RDX
Wire Wire Line
	4550 3150 4100 3150
Wire Wire Line
	4100 3150 4100 3050
Wire Wire Line
	4550 3050 4100 3050
Connection ~ 4100 3050
Wire Wire Line
	4450 4150 4450 4300
Wire Wire Line
	4100 3150 4100 3600
Connection ~ 4100 3150
Wire Wire Line
	4450 2200 4450 3250
Wire Wire Line
	4100 1200 4100 3050
Wire Wire Line
	3200 2300 4550 2300
Wire Wire Line
	3300 2400 4550 2400
Wire Wire Line
	2800 1200 4100 1200
$Comp
L ddcmp_symbols:R_US R7
U 1 1 60BD0969
P 6550 2800
F 0 "R7" H 6618 2846 50  0000 L CNN
F 1 "51" H 6618 2755 50  0000 L CNN
F 2 "ddcmp_parts:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 6590 2790 50  0001 C CNN
F 3 "" H 6550 2800 50  0001 C CNN
	1    6550 2800
	1    0    0    -1  
$EndComp
$Comp
L ddcmp_symbols:R_US R6
U 1 1 60BD1A0F
P 3100 3100
F 0 "R6" H 3168 3146 50  0000 L CNN
F 1 "51" H 3168 3055 50  0000 L CNN
F 2 "ddcmp_parts:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 3140 3090 50  0001 C CNN
F 3 "" H 3100 3100 50  0001 C CNN
	1    3100 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6550 2950 6550 3000
Connection ~ 6550 3000
Wire Wire Line
	6550 3000 6400 3000
Wire Wire Line
	6550 2650 6550 2600
Connection ~ 6550 2600
Wire Wire Line
	6550 2600 6750 2600
Wire Wire Line
	3100 2950 3100 2900
Connection ~ 3100 2900
Wire Wire Line
	3100 2900 3200 2900
Wire Wire Line
	3100 3250 3100 3300
Connection ~ 3100 3300
Wire Wire Line
	3100 3300 3300 3300
$EndSCHEMATC
