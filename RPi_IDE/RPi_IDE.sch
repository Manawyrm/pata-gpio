EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J2
U 1 1 5F32CFA6
P 5930 4030
F 0 "J2" H 5980 5147 50  0000 C CNN
F 1 "IDE" H 5980 5056 50  0000 C CNN
F 2 "Connector_IDC:IDC-Header_2x20_P2.54mm_Latch_Vertical" H 5930 4030 50  0001 C CNN
F 3 "~" H 5930 4030 50  0001 C CNN
	1    5930 4030
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR01
U 1 1 5F32FDE1
P 3200 2565
F 0 "#PWR01" H 3200 2415 50  0001 C CNN
F 1 "+3.3V" H 3215 2738 50  0000 C CNN
F 2 "" H 3200 2565 50  0001 C CNN
F 3 "" H 3200 2565 50  0001 C CNN
	1    3200 2565
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR02
U 1 1 5F330583
P 5120 3000
F 0 "#PWR02" H 5120 2850 50  0001 C CNN
F 1 "+3.3V" H 5135 3173 50  0000 C CNN
F 2 "" H 5120 3000 50  0001 C CNN
F 3 "" H 5120 3000 50  0001 C CNN
	1    5120 3000
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR03
U 1 1 5F330877
P 6555 4030
F 0 "#PWR03" H 6555 3880 50  0001 C CNN
F 1 "+3.3V" V 6550 4240 50  0000 C CNN
F 2 "" H 6555 4030 50  0001 C CNN
F 3 "" H 6555 4030 50  0001 C CNN
	1    6555 4030
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 5F330BDD
P 3000 5550
F 0 "#PWR05" H 3000 5300 50  0001 C CNN
F 1 "GND" H 3005 5377 50  0000 C CNN
F 2 "" H 3000 5550 50  0001 C CNN
F 3 "" H 3000 5550 50  0001 C CNN
	1    3000 5550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 5F33123F
P 5985 5385
F 0 "#PWR04" H 5985 5135 50  0001 C CNN
F 1 "GND" H 5990 5212 50  0000 C CNN
F 2 "" H 5985 5385 50  0001 C CNN
F 3 "" H 5985 5385 50  0001 C CNN
	1    5985 5385
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C1
U 1 1 5F3316F9
P 5120 4380
F 0 "C1" H 5310 4425 50  0000 R CNN
F 1 "1µF" H 5365 4325 50  0000 R CNN
F 2 "Capacitors_THT:CP_Radial_D10.0mm_P5.00mm" H 5120 4380 50  0001 C CNN
F 3 "~" H 5120 4380 50  0001 C CNN
	1    5120 4380
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Raspberry_Pi_2_3 J1
U 1 1 5F32ABA5
P 3050 3995
F 0 "J1" H 3050 5476 50  0000 C CNN
F 1 "Raspberry_Pi_2_3" H 2700 5390 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x20_Pitch2.54mm" H 3050 3995 50  0001 C CNN
F 3 "https://www.raspberrypi.org/documentation/hardware/raspberrypi/schematics/rpi_SCH_3bplus_1p0_reduced.pdf" H 3050 3995 50  0001 C CNN
	1    3050 3995
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 2695 3150 2600
Wire Wire Line
	3150 2600 3200 2600
Wire Wire Line
	3250 2600 3250 2695
Wire Wire Line
	3200 2565 3200 2600
Connection ~ 3200 2600
Wire Wire Line
	3200 2600 3250 2600
Wire Wire Line
	2650 5295 2650 5415
Wire Wire Line
	2650 5415 2750 5415
Wire Wire Line
	3350 5415 3350 5295
Wire Wire Line
	3000 5550 3000 5415
Connection ~ 3000 5415
Wire Wire Line
	3000 5415 3050 5415
Wire Wire Line
	2750 5295 2750 5415
Connection ~ 2750 5415
Wire Wire Line
	2750 5415 2850 5415
Wire Wire Line
	2850 5295 2850 5415
Connection ~ 2850 5415
Wire Wire Line
	2950 5295 2950 5415
Wire Wire Line
	2850 5415 2950 5415
Connection ~ 2950 5415
Wire Wire Line
	2950 5415 3000 5415
Wire Wire Line
	3050 5295 3050 5415
Connection ~ 3050 5415
Wire Wire Line
	3050 5415 3150 5415
Wire Wire Line
	3150 5295 3150 5415
Connection ~ 3150 5415
Wire Wire Line
	3150 5415 3250 5415
Wire Wire Line
	3250 5295 3250 5415
Connection ~ 3250 5415
Wire Wire Line
	3250 5415 3350 5415
Wire Wire Line
	5650 3130 5730 3130
Text GLabel 5650 3130 0    50   Input ~ 0
RESET
Wire Wire Line
	5985 5235 5985 5385
Connection ~ 5985 5235
Wire Wire Line
	6310 4930 6230 4930
Text GLabel 6310 4930 2    50   Input ~ 0
CS1
Wire Wire Line
	6655 5235 6655 5030
Wire Wire Line
	6655 4130 6230 4130
Connection ~ 6655 5030
Wire Wire Line
	6655 5030 6655 4530
Wire Wire Line
	5985 5235 6655 5235
Wire Wire Line
	6230 5030 6655 5030
Wire Wire Line
	6310 4830 6230 4830
NoConn ~ 6230 4730
Text GLabel 6310 4830 2    50   Input ~ 0
DA2
NoConn ~ 6230 4630
Wire Wire Line
	6230 4530 6655 4530
Connection ~ 6655 4530
NoConn ~ 6230 4430
Wire Wire Line
	6230 4330 6655 4330
Connection ~ 6655 4330
Wire Wire Line
	6655 4330 6655 4530
Wire Wire Line
	6230 4230 6655 4230
Wire Wire Line
	6655 4130 6655 4230
Connection ~ 6655 4230
Wire Wire Line
	6655 4230 6655 4330
Wire Wire Line
	6230 3130 6655 3130
Wire Wire Line
	6655 3130 6655 4130
Connection ~ 6655 4130
Wire Wire Line
	6310 3930 6230 3930
Text GLabel 6310 3930 2    50   Input ~ 0
D15
Wire Wire Line
	6230 4030 6555 4030
Wire Wire Line
	6310 3830 6230 3830
Wire Wire Line
	6310 3730 6230 3730
Wire Wire Line
	6310 3630 6230 3630
Wire Wire Line
	6310 3530 6230 3530
Wire Wire Line
	6310 3430 6230 3430
Wire Wire Line
	6310 3330 6230 3330
Wire Wire Line
	6310 3230 6230 3230
Text GLabel 6310 3830 2    50   Input ~ 0
D14
Text GLabel 6310 3730 2    50   Input ~ 0
D13
Text GLabel 6310 3630 2    50   Input ~ 0
D12
Text GLabel 6310 3530 2    50   Input ~ 0
D11
Text GLabel 6310 3430 2    50   Input ~ 0
D10
Text GLabel 6310 3330 2    50   Input ~ 0
D9
Text GLabel 6310 3230 2    50   Input ~ 0
D8
Wire Wire Line
	5650 3230 5730 3230
Wire Wire Line
	5650 3330 5730 3330
Wire Wire Line
	5650 3430 5730 3430
Wire Wire Line
	5650 3530 5730 3530
Wire Wire Line
	5650 3630 5730 3630
Wire Wire Line
	5650 3730 5730 3730
Wire Wire Line
	5650 3830 5730 3830
Wire Wire Line
	5650 3930 5730 3930
Wire Wire Line
	5650 4030 5730 4030
Text GLabel 5650 4030 0    50   Input ~ 0
GND
Text GLabel 5650 3930 0    50   Input ~ 0
D0
Text GLabel 5650 3830 0    50   Input ~ 0
D1
Text GLabel 5650 3730 0    50   Input ~ 0
D2
Text GLabel 5650 3630 0    50   Input ~ 0
D3
Text GLabel 5650 3530 0    50   Input ~ 0
D4
Text GLabel 5650 3430 0    50   Input ~ 0
D5
Text GLabel 5650 3330 0    50   Input ~ 0
D6
Text GLabel 5650 3230 0    50   Input ~ 0
D7
Wire Wire Line
	5650 4230 5730 4230
Wire Wire Line
	5650 4330 5730 4330
Wire Wire Line
	5650 4430 5730 4430
Wire Wire Line
	5650 4530 5730 4530
Wire Wire Line
	5650 4630 5730 4630
Wire Wire Line
	5650 4730 5730 4730
Wire Wire Line
	5650 4830 5730 4830
Wire Wire Line
	5650 4930 5730 4930
Wire Wire Line
	5650 5030 5730 5030
Text GLabel 5650 4230 0    50   Input ~ 0
IOW
Text GLabel 5650 4330 0    50   Input ~ 0
IOR
Text GLabel 5650 4430 0    50   Input ~ 0
IO_CH_RD
Wire Wire Line
	5120 4230 5120 4130
Connection ~ 5120 4130
Wire Wire Line
	5120 4130 5120 3000
Wire Wire Line
	5120 4130 5730 4130
Wire Wire Line
	5120 4530 5120 5235
Wire Wire Line
	5120 5235 5985 5235
Text GLabel 5650 4530 0    50   Input ~ 0
DMA_ACK
Text GLabel 5650 4630 0    50   Input ~ 0
IRQ
Text GLabel 5650 4730 0    50   Input ~ 0
DA1
Text GLabel 5650 4830 0    50   Input ~ 0
DA0
Text GLabel 5650 4930 0    50   Input ~ 0
CS0
Text GLabel 5650 5030 0    50   Input ~ 0
ACTIVE
Wire Wire Line
	2170 3095 2250 3095
Wire Wire Line
	2170 3195 2250 3195
Text GLabel 2170 3095 0    50   Input ~ 0
D8
Text GLabel 2170 3195 0    50   Input ~ 0
D9
Wire Wire Line
	2170 3495 2250 3495
Wire Wire Line
	2170 3595 2250 3595
Text GLabel 2170 3495 0    50   Input ~ 0
D5
Text GLabel 2170 3595 0    50   Input ~ 0
D10
Wire Wire Line
	2170 3795 2250 3795
Text GLabel 2170 3795 0    50   Input ~ 0
CS0
Wire Wire Line
	2170 4195 2250 4195
Wire Wire Line
	2170 4295 2250 4295
Wire Wire Line
	2170 4395 2250 4395
Wire Wire Line
	2170 4495 2250 4495
Text GLabel 2170 4195 0    50   Input ~ 0
D3
Text GLabel 2170 4295 0    50   Input ~ 0
D11
Text GLabel 2170 4395 0    50   Input ~ 0
D12
Text GLabel 2170 4495 0    50   Input ~ 0
D13
Wire Wire Line
	2170 4695 2250 4695
Text GLabel 2170 4695 0    50   Input ~ 0
D4
Wire Wire Line
	3930 3095 3850 3095
Wire Wire Line
	3930 3195 3850 3195
Wire Wire Line
	3930 3395 3850 3395
Wire Wire Line
	3930 3495 3850 3495
Text GLabel 3930 3095 2    50   Input ~ 0
IOW
Text GLabel 3930 3195 2    50   Input ~ 0
DA2
Text GLabel 3930 3395 2    50   Input ~ 0
RESET
Text GLabel 3930 3495 2    50   Input ~ 0
D7
Wire Wire Line
	3930 3695 3850 3695
Wire Wire Line
	3930 3795 3850 3795
Wire Wire Line
	3930 3895 3850 3895
Wire Wire Line
	3930 4095 3850 4095
Wire Wire Line
	3930 4195 3850 4195
Wire Wire Line
	3930 4295 3850 4295
Text GLabel 3930 3695 2    50   Input ~ 0
D6
Text GLabel 3930 3795 2    50   Input ~ 0
IOR
Text GLabel 3930 3895 2    50   Input ~ 0
DA1
Text GLabel 3930 4095 2    50   Input ~ 0
D15
Text GLabel 3930 4195 2    50   Input ~ 0
D14
Text GLabel 3930 4295 2    50   Input ~ 0
D1
Wire Wire Line
	3930 4395 3850 4395
Wire Wire Line
	3930 4495 3850 4495
Wire Wire Line
	3930 4695 3850 4695
Wire Wire Line
	3930 4795 3850 4795
Text GLabel 3930 4395 2    50   Input ~ 0
D2
Text GLabel 3930 4495 2    50   Input ~ 0
D0
Text GLabel 3930 4695 2    50   Input ~ 0
CS1
Text GLabel 3930 4795 2    50   Input ~ 0
DA0
$EndSCHEMATC
