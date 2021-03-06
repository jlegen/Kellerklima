EESchema Schematic File Version 4
LIBS:SensorPCB_V1.2-cache
EELAYER 26 0
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
L SensorPCB_V1.2-rescue:P82B715D-philips-SensorPCB_V1.2-rescue U4
U 1 1 5A6FDC5B
P 3700 1650
F 0 "U4" H 3700 2267 50  0000 C CNN
F 1 "P82B715D" H 3700 2176 50  0000 C CNN
F 2 "Package_SO:SO-8_5.3x6.2mm_P1.27mm" H 3700 1650 50  0001 C CNN
F 3 "http://www.nxp.com/documents/data_sheet/P82B96.pdf" H 3700 1650 50  0001 C CNN
	1    3700 1650
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x04 J02
U 1 1 5A6FDDC2
P 5850 1950
F 0 "J02" H 5800 2150 50  0000 L CNN
F 1 "MCU" H 5800 1650 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_bornier-4_P5.08mm" H 5850 1950 50  0001 C CNN
F 3 "~" H 5850 1950 50  0001 C CNN
	1    5850 1950
	1    0    0    -1  
$EndComp
$Comp
L device:R R4
U 1 1 5A6FDF0E
P 4750 950
F 0 "R4" H 4820 996 50  0000 L CNN
F 1 "470" H 4820 905 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 4680 950 50  0001 C CNN
F 3 "" H 4750 950 50  0001 C CNN
	1    4750 950 
	1    0    0    -1  
$EndComp
$Comp
L device:R R5
U 1 1 5A6FDF95
P 5150 950
F 0 "R5" H 5220 996 50  0000 L CNN
F 1 "470" H 5220 905 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 5080 950 50  0001 C CNN
F 3 "" H 5150 950 50  0001 C CNN
	1    5150 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 1750 4450 1750
Wire Wire Line
	4450 1750 4450 2350
Wire Wire Line
	4450 2350 2200 2350
Wire Wire Line
	2200 2350 2200 1450
Wire Wire Line
	3100 1750 2450 1750
Wire Wire Line
	2850 1950 3100 1950
Wire Wire Line
	3100 1550 2950 1550
Wire Wire Line
	2950 1550 2950 1100
Wire Wire Line
	2950 1100 4750 1100
$Comp
L power:VCC #PWR010
U 1 1 5A6F2DFD
P 4300 750
F 0 "#PWR010" H 4300 600 50  0001 C CNN
F 1 "VCC" H 4317 923 50  0000 C CNN
F 2 "" H 4300 750 50  0001 C CNN
F 3 "" H 4300 750 50  0001 C CNN
	1    4300 750 
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR012
U 1 1 5A6F3247
P 5000 3000
F 0 "#PWR012" H 5000 2850 50  0001 C CNN
F 1 "VCC" H 5000 3150 50  0000 C CNN
F 2 "" H 5000 3000 50  0001 C CNN
F 3 "" H 5000 3000 50  0001 C CNN
	1    5000 3000
	1    0    0    -1  
$EndComp
$Comp
L device:D_Zener D1
U 1 1 5A6F37CD
P 5300 3150
F 0 "D1" V 5300 3000 50  0000 L CNN
F 1 "5V1" V 5400 3000 50  0000 L CNN
F 2 "Diode_THT:D_A-405_P7.62mm_Horizontal" H 5300 3150 50  0001 C CNN
F 3 "https://en.wikipedia.org/wiki/Zener_diode" H 5300 3150 50  0001 C CNN
	1    5300 3150
	0    1    1    0   
$EndComp
Connection ~ 5300 3300
$Comp
L sensors:DHT11 U2
U 1 1 5A6F452C
P 1100 2550
F 0 "U2" H 872 2596 50  0000 R CNN
F 1 "DHT22" H 872 2505 50  0000 R CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 1250 2800 50  0001 C CNN
F 3 "http://akizukidenshi.com/download/ds/aosong/DHT11.pdf" H 1250 2800 50  0001 C CNN
	1    1100 2550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 5A6F45AA
P 1150 2850
F 0 "#PWR04" H 1150 2600 50  0001 C CNN
F 1 "GND" H 1155 2677 50  0000 C CNN
F 2 "" H 1150 2850 50  0001 C CNN
F 3 "" H 1150 2850 50  0001 C CNN
	1    1150 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 2850 1150 2850
Wire Wire Line
	1400 2550 1650 2550
$Comp
L device:R R1
U 1 1 5A6F5442
P 1650 2300
F 0 "R1" H 1720 2346 50  0000 L CNN
F 1 "10k" H 1720 2255 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 1580 2300 50  0001 C CNN
F 3 "" H 1650 2300 50  0001 C CNN
	1    1650 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 2450 1650 2550
Connection ~ 1650 2550
Wire Wire Line
	1550 2250 1550 2150
Wire Wire Line
	1550 2150 1650 2150
$Comp
L SensorPCB_V1.2-rescue:SHT1x-sensors-SensorPCB_V1.2-rescue U1
U 1 1 5A6F66FE
P 1100 1350
F 0 "U1" H 872 1396 50  0000 R CNN
F 1 "SHT31" H 872 1305 50  0000 R CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 1250 1600 50  0001 C CNN
F 3 "https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/Humidity_Sensors/Sensirion_Humidity_Sensors_SHT1x_Datasheet_V5.pdf" H 1250 1600 50  0001 C CNN
	1    1100 1350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5A6F6788
P 1150 1750
F 0 "#PWR03" H 1150 1500 50  0001 C CNN
F 1 "GND" H 1155 1577 50  0000 C CNN
F 2 "" H 1150 1750 50  0001 C CNN
F 3 "" H 1150 1750 50  0001 C CNN
	1    1150 1750
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR01
U 1 1 5A6F67AB
P 2100 1600
F 0 "#PWR01" H 2100 1450 50  0001 C CNN
F 1 "VCC" H 2100 1550 50  0000 C CNN
F 2 "" H 2100 1600 50  0001 C CNN
F 3 "" H 2100 1600 50  0001 C CNN
	1    2100 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1400 1450 2200 1450
Wire Wire Line
	1000 1650 1150 1650
$Comp
L device:R R3
U 1 1 5A6F87B5
P 2300 950
F 0 "R3" H 2370 996 50  0000 L CNN
F 1 "4k7" H 2370 905 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 2230 950 50  0001 C CNN
F 3 "" H 2300 950 50  0001 C CNN
	1    2300 950 
	1    0    0    -1  
$EndComp
$Comp
L device:R R2
U 1 1 5A6F8836
P 1900 950
F 0 "R2" H 1970 996 50  0000 L CNN
F 1 "4k7" H 1970 905 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 1830 950 50  0001 C CNN
F 3 "" H 1900 950 50  0001 C CNN
	1    1900 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 1100 1900 1250
$Comp
L SensorPCB_V1.2-rescue:Mounting_Hole-mechanical-SensorPCB_V1.2-rescue MK1
U 1 1 5A6FB100
P 650 650
F 0 "MK1" H 750 696 50  0000 L CNN
F 1 "Mounting_Hole" H 750 605 50  0000 L CNN
F 2 "Mounting_Holes:MountingHole_2.2mm_M2" H 650 650 50  0001 C CNN
F 3 "" H 650 650 50  0001 C CNN
	1    650  650 
	1    0    0    -1  
$EndComp
$Comp
L SensorPCB_V1.2-rescue:Mounting_Hole-mechanical-SensorPCB_V1.2-rescue MK2
U 1 1 5A6FB293
P 650 3350
F 0 "MK2" H 750 3396 50  0000 L CNN
F 1 "Mounting_Hole" H 750 3305 50  0000 L CNN
F 2 "Mounting_Holes:MountingHole_2.2mm_M2" H 650 3350 50  0001 C CNN
F 3 "" H 650 3350 50  0001 C CNN
	1    650  3350
	1    0    0    -1  
$EndComp
$Comp
L SensorPCB_V1.2-rescue:LM1117-3.3-AMS1117-3V3-SensorPCB_V1.2-rescue U3
U 1 1 5A73594C
P 2550 3100
F 0 "U3" H 2550 3374 50  0000 C CNN
F 1 "LM1117-3.3" H 2550 3465 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 2550 3557 50  0001 C CNN
F 3 "" H 2550 3557 50  0000 C CNN
	1    2550 3100
	-1   0    0    1   
$EndComp
Wire Wire Line
	2250 3100 1950 3100
$Comp
L device:C C2
U 1 1 5A73A127
P 3400 3250
F 0 "C2" H 3515 3296 50  0000 L CNN
F 1 "100n" H 3515 3205 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 3438 3100 50  0001 C CNN
F 3 "" H 3400 3250 50  0001 C CNN
	1    3400 3250
	1    0    0    -1  
$EndComp
$Comp
L device:CP C1
U 1 1 5A73A2AC
P 1950 3250
F 0 "C1" H 2068 3296 50  0000 L CNN
F 1 "100uF" H 2068 3205 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D4.0mm_P2.00mm" H 1988 3100 50  0001 C CNN
F 3 "" H 1950 3250 50  0001 C CNN
	1    1950 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 3400 1950 3650
Wire Wire Line
	1950 3650 3400 3650
Wire Wire Line
	3400 3650 3400 3400
Wire Wire Line
	3400 3650 4100 3650
Connection ~ 3400 3650
$Comp
L SensorPCB_V1.2-rescue:Conn_01x03-Connector J3
U 1 1 5A75E08D
P 1650 1700
F 0 "J3" H 1600 1500 50  0000 L CNN
F 1 "3.3V/5V" V 1750 1550 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 1650 1700 50  0001 C CNN
F 3 "~" H 1650 1700 50  0001 C CNN
	1    1650 1700
	-1   0    0    1   
$EndComp
Wire Wire Line
	700  1700 700  1050
Wire Wire Line
	700  1050 1000 1050
Wire Wire Line
	1000 2250 1550 2250
Wire Wire Line
	1000 1700 1000 2250
Wire Wire Line
	1900 800  2300 800 
Wire Wire Line
	1000 1050 1000 800 
Connection ~ 1000 1050
Connection ~ 1900 800 
Wire Wire Line
	1150 1650 1150 1750
Text Label 4700 1700 0    50   ~ 0
SDA
Text Label 5050 1600 0    50   ~ 0
DHT
Text Notes 5950 1900 0    50   ~ 0
SDA/DHT
Text Notes 5950 2000 0    50   ~ 0
SCK
Text Notes 5950 2200 0    50   ~ 0
GND
Text Notes 5950 2100 0    50   ~ 0
5V
Wire Wire Line
	1900 1250 1400 1250
Connection ~ 2200 1450
Wire Wire Line
	2300 1100 2300 1450
Wire Wire Line
	2200 1450 2300 1450
Wire Wire Line
	2450 1750 2450 1250
Wire Wire Line
	2450 1250 1900 1250
Connection ~ 1900 1250
Wire Wire Line
	4750 800  4950 800 
Wire Wire Line
	4950 650  4950 800 
Connection ~ 4950 800 
Wire Wire Line
	4950 800  5150 800 
$Comp
L SensorPCB_V1.2-rescue:Conn_01x02-Connector-SensorPCB_V1.2-rescue J6
U 1 1 5BAB2A54
P 5150 550
F 0 "J6" H 5230 542 50  0000 L CNN
F 1 "Pullup Line" H 5230 451 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 5150 550 50  0001 C CNN
F 3 "~" H 5150 550 50  0001 C CNN
	1    5150 550 
	1    0    0    -1  
$EndComp
$Comp
L SensorPCB_V1.2-rescue:Conn_01x02-Connector-SensorPCB_V1.2-rescue J5
U 1 1 5BAB2BF9
P 1500 600
F 0 "J5" V 1466 412 50  0000 R CNN
F 1 "Pullup I2C" V 1375 412 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 1500 600 50  0001 C CNN
F 3 "~" H 1500 600 50  0001 C CNN
	1    1500 600 
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1600 800  1900 800 
Wire Wire Line
	1000 800  1500 800 
Connection ~ 1000 2250
Wire Wire Line
	1000 1700 700  1700
Connection ~ 1950 3100
Wire Wire Line
	1850 1600 2100 1600
Wire Wire Line
	1850 1800 1950 1800
Wire Wire Line
	1950 1800 1950 3100
Wire Wire Line
	1650 2150 1650 1950
Wire Wire Line
	1650 1950 2000 1950
Wire Wire Line
	2000 1950 2000 1700
Wire Wire Line
	2000 1700 1850 1700
Connection ~ 1650 2150
$Comp
L SensorPCB_V1.2-rescue:Conn_01x03-Connector J1
U 1 1 5C596B9D
P 4950 1400
F 0 "J1" V 4950 1600 50  0000 L CNN
F 1 "DHT/I2C" V 5050 1250 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 4950 1400 50  0001 C CNN
F 3 "~" H 4950 1400 50  0001 C CNN
	1    4950 1400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4750 1600 4850 1600
Wire Wire Line
	1650 2550 5050 2550
Wire Wire Line
	5650 1950 5150 1950
Wire Wire Line
	4600 1950 4600 1550
Wire Wire Line
	4600 1550 4300 1550
Wire Wire Line
	5650 1850 4950 1850
Wire Wire Line
	4950 1850 4950 1600
Wire Wire Line
	5050 2550 5050 1600
Wire Wire Line
	4750 1100 4750 1600
Connection ~ 4750 1100
Wire Wire Line
	5150 1100 5150 1950
Connection ~ 5150 1950
Wire Wire Line
	5150 1950 4600 1950
Wire Wire Line
	5300 3000 5450 3000
Wire Wire Line
	5450 3000 5450 2750
Wire Wire Line
	5450 2050 5650 2050
Connection ~ 5300 3000
Wire Wire Line
	5650 2150 5650 2750
Wire Wire Line
	5300 3300 5650 3300
$Comp
L device:CP C3
U 1 1 5C5F543E
P 5000 3150
F 0 "C3" H 4750 3150 50  0000 L CNN
F 1 "100uF" H 4750 3050 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D4.0mm_P2.00mm" H 5038 3000 50  0001 C CNN
F 3 "" H 5000 3150 50  0001 C CNN
	1    5000 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4950 550  4450 550 
Wire Wire Line
	4450 550  4450 750 
Wire Wire Line
	4450 750  4300 750 
Wire Wire Line
	2550 2800 4100 2800
Wire Wire Line
	4100 2800 4100 3650
$Comp
L power:GND #PWR0101
U 1 1 5C6224B2
P 5000 3300
F 0 "#PWR0101" H 5000 3050 50  0001 C CNN
F 1 "GND" H 5005 3127 50  0000 C CNN
F 2 "" H 5000 3300 50  0001 C CNN
F 3 "" H 5000 3300 50  0001 C CNN
	1    5000 3300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5C62B51A
P 3400 3650
F 0 "#PWR0102" H 3400 3400 50  0001 C CNN
F 1 "GND" H 3405 3477 50  0000 C CNN
F 2 "" H 3400 3650 50  0001 C CNN
F 3 "" H 3400 3650 50  0001 C CNN
	1    3400 3650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5C62E644
P 2850 2100
F 0 "#PWR0103" H 2850 1850 50  0001 C CNN
F 1 "GND" H 2855 1927 50  0000 C CNN
F 2 "" H 2850 2100 50  0001 C CNN
F 3 "" H 2850 2100 50  0001 C CNN
	1    2850 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 1950 2850 2100
$Comp
L power:VCC #PWR0104
U 1 1 5C630007
P 3400 3100
F 0 "#PWR0104" H 3400 2950 50  0001 C CNN
F 1 "VCC" H 3417 3273 50  0000 C CNN
F 2 "" H 3400 3100 50  0001 C CNN
F 3 "" H 3400 3100 50  0001 C CNN
	1    3400 3100
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR0105
U 1 1 5C631A26
P 4500 1350
F 0 "#PWR0105" H 4500 1200 50  0001 C CNN
F 1 "VCC" H 4517 1523 50  0000 C CNN
F 2 "" H 4500 1350 50  0001 C CNN
F 3 "" H 4500 1350 50  0001 C CNN
	1    4500 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 1350 4500 1350
Connection ~ 3400 3100
Wire Wire Line
	2850 3100 3400 3100
Wire Wire Line
	5000 3000 5300 3000
Wire Wire Line
	5000 3300 5300 3300
Connection ~ 5000 3000
Connection ~ 5000 3300
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 5C5D964A
P 5850 2750
F 0 "#FLG0101" H 5850 2825 50  0001 C CNN
F 1 "PWR_FLAG" H 5850 2924 50  0000 C CNN
F 2 "" H 5850 2750 50  0001 C CNN
F 3 "~" H 5850 2750 50  0001 C CNN
	1    5850 2750
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 5C5D9698
P 5550 2750
F 0 "#FLG0102" H 5550 2825 50  0001 C CNN
F 1 "PWR_FLAG" H 5550 2924 50  0000 C CNN
F 2 "" H 5550 2750 50  0001 C CNN
F 3 "~" H 5550 2750 50  0001 C CNN
	1    5550 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 2750 5650 2750
Connection ~ 5650 2750
Wire Wire Line
	5650 2750 5650 3300
Wire Wire Line
	5550 2750 5450 2750
Connection ~ 5450 2750
Wire Wire Line
	5450 2750 5450 2050
$Comp
L power:PWR_FLAG #FLG0103
U 1 1 5C5DC462
P 2100 1950
F 0 "#FLG0103" H 2100 2025 50  0001 C CNN
F 1 "PWR_FLAG" H 2100 2124 50  0000 C CNN
F 2 "" H 2100 1950 50  0001 C CNN
F 3 "~" H 2100 1950 50  0001 C CNN
	1    2100 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 1950 2000 1950
Connection ~ 2000 1950
$EndSCHEMATC
