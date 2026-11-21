# ECE 528/L - Robotics and Embedded Systems with Lab
**CSU Northridge**

**Department of Electrical and Computer Engineering**

## Bluetooth Low Energy (BLE) Lab
The Bluetooth Low Energy (BLE) lab interfaces with the following:

* User LEDs of the TI MSP432 LaunchPad
* Pololu Gearmotor with Encoder - [Product Link](https://www.pololu.com/product/3675)
* Adafruit Bluefruit LE UART Friend - Bluetooth Low Energy (BLE) Module - [Product Link](https://www.adafruit.com/product/2479)

## Pre-Lab

### 1. What is Bluetooth Low Energy (BLE)? How does it differ from regular Bluetooth in terms of power consumption and range?

Bluetooth Low Energy is a wireless protocol in the Bluetooth standard for sending small amounts of data while leaving the radio asleep most of the time. A device wakes for a short advertising or connection event, exchanges a packet, and returns to sleep [1], [2].

Bluetooth Classic (BR/EDR) is built for a continuous link, such as audio streaming, so the radio stays active for much longer intervals and the average current is higher. BLE’s short duty cycle is what cuts the average power. Both radios use the 2.4 GHz band. Range depends on transmit power, receiver sensitivity, the antenna, and the surroundings, and for a typical implementation it is on the order of tens of meters for both. BLE is specified for lower average power and a lower data rate, with a similar practical range [1], [2].

### 2. What does GAP stand for and what is its purpose? What is the difference between Peripheral and Central devices in the context of BLE?

GAP stands for Generic Access Profile. It defines how two devices become aware of each other and how a connection is formed: advertising, scanning, roles, and the parameters used to start and maintain a link [1], [2].

A Peripheral advertises and accepts a connection. It is usually the lower-power device, such as the Bluefruit module in this lab. A Central scans for those advertisements and sends the connection request. It is usually a phone or a computer [2].

### 3. What is GATT in BLE, and how does it structure data exchange between connected devices? Provide definitions for the following: Profile, Service, and Characteristic.

GATT stands for Generic Attribute Profile. After GAP has a connection, GATT is the client/server protocol used to find and transfer data. The GATT server holds the data. The GATT client reads it, writes it, or subscribes to updates. The data is arranged as a hierarchy of profile, service, and characteristic [1], [2].

| Term | Definition |
|---|---|
| Profile | An application specification that lists the services a device offers for one use, such as a heart-rate or battery use case. |
| Service | A group of related data items, identified by a UUID. A battery service is one example. |
| Characteristic | One data value inside a service, also identified by a UUID. It has properties such as read, write, or notify, and it may include descriptors. |

### 4. What is an advertising packet in BLE and what information can it contain?

An advertising packet is the broadcast a device sends while it is not in a connection, so a Central can discover it and decide whether to connect. The legacy advertising payload is at most 31 bytes. It can carry flags, the local name, transmit power, service UUIDs, service data, appearance, and manufacturer-specific data, together with the advertiser’s device address [1], [2].

### 5. What is a UUID used for in BLE and why are 128-bit UUIDs used?

A UUID (universally unique identifier) names a GATT service, characteristic, or descriptor so both devices agree on what a value means. The Bluetooth SIG assigns short 16-bit UUIDs to adopted services. Each of those is a shorthand for one reserved 128-bit base UUID. A custom service or characteristic uses a full 128-bit UUID so its identifier does not collide with a SIG-assigned number or with another vendor’s custom service [1], [2].

### 6. What bit from which register determines if the UART data is received or transmitted MSB or LSB first?

Bit 13 of the eUSCI_A Control Word Register 0, `UCAxCTLW0`, selects the order. That bit is named `UCMSB` [3].

| `UCMSB` (bit 13) | Bit order |
|---|---|
| 0 | LSB first |
| 1 | MSB first |

The same bit applies to data shifted in on receive and shifted out on transmit.

### 7. Mnemonic and primary module function for each pin

From Table 6-45 (Default Mapping) of the MSP432P401R datasheet. A receive-data pin’s primary function is an input. A transmit-data pin’s primary function is an output [4].

| Pin | Mnemonic | Input Pin Primary Function | Output Pin Primary Function |
|---|---|---|---|
| P2.2 | `PM_UCA1RXD` | `UCA1RXD` | N/A |
| P2.3 | `PM_UCA1TXD` | N/A | `UCA1TXD` |
| P3.2 | `PM_UCA2RXD` | `UCA2RXD` | N/A |
| P3.3 | `PM_UCA2TXD` | N/A | `UCA2TXD` |

`UCA1RXD` and `UCA1TXD` are the eUSCI_A1 UART pins. `UCA2RXD` and `UCA2TXD` are the eUSCI_A2 UART pins. Section 4.1 of the datasheet shows these alternate functions on the pin diagrams, and Section 6.12.2 describes P2.2 and P2.3 as Port P2 pins with a Schmitt-trigger input [4].

### 8. `P2_2_and_P2_3_Init`

Section 12.2.6 of the Technical Reference Manual defines the function-select encoding [3]:

| `PxSEL1` | `PxSEL0` | Function |
|---|---|---|
| 0 | 0 | GPIO |
| 0 | 1 | Primary module function |
| 1 | 0 | Secondary module function |
| 1 | 1 | Tertiary module function |

P2.2 is bit 2 and P2.3 is bit 3, so the mask is `0x0C`. Primary mode sets `SEL0` and clears `SEL1`. Direction is left unchanged, as the question allows.

```c
void P2_2_and_P2_3_Init()
{
    // Primary module function for P2.2 and P2.3: SEL1 = 0, SEL0 = 1
    P2->SEL0 |= 0x0C;
    P2->SEL1 &= ~0x0C;
}
```

## Overview

This lab connects an Adafruit Bluefruit LE UART Friend to the MSP432 LaunchPad over UART and uses that link to control the TI-RSLK MAX. EUSCI_A3 is configured for 9600 baud, 8 data bits, no parity, one stop bit, and LSB first. The phone runs the Bluefruit Connect app as the BLE central. The module is the peripheral.

With `LAB_MODE` set to 0, a text command from the UART screen turns the LaunchPad RGB LED green or off, and the Control Pad arrow buttons drive the chassis. Pressing an arrow runs both motors at 30% duty cycle. Releasing the button stops both motors. `LAB_MODE` 1 transmits a fixed four-byte pattern for the oscilloscope. `LAB_MODE` 2 transmits one byte selected by the PMOD SWT switches.

## Components Used

| Component | Quantity | Manufacturer |
|---|---|---|
| MSP432P401R LaunchPad | 1 | Texas Instruments |
| USB-A to Micro-USB cable | 1 | N/A |
| TI-RSLK MAX chassis with motor-driver board | 1 | Pololu |
| [Pololu gearmotors with encoders](https://www.pololu.com/product/3675) | 2 | Pololu |
| [Adafruit Bluefruit LE UART Friend](https://www.adafruit.com/product/2479) | 1 | Adafruit |
| PMOD SWT (4 slide switches) | 1 | Digilent |
| LaunchPad RGB LED | Built in | Texas Instruments |
| Oscilloscope and probe | 1 | N/A |
| AA batteries | 6 | N/A |
| Phone with the Bluefruit Connect app | 1 | N/A |
| Computer with Code Composer Studio | 1 | Texas Instruments |

**Main signal connections**

| Bluefruit pin | LaunchPad pin |
|---|---|
| MOD | P1.6 |
| CTS | GND |
| TXO | P9.6 (`PM_UCA3RXD`) |
| RXI | P9.7 (`PM_UCA3TXD`) |
| VIN | 3.3 V |
| RTS | Not connected |
| GND | GND |
| DFU | Not connected |

| Other signal | LaunchPad pin(s) |
|---|---|
| PMOD SWT SW1–SW4 | P10.0–P10.3 |
| Right / left motor PWM | P2.6 / P2.7 |
| Left / right motor direction | P5.4 / P5.5 |
| Motor enable (nSLEEP) | P3.6–P3.7 |
| RGB LED | P2.0–P2.2 |
| Application UART (serial terminal) | P1.2 / P1.3 |

CTS is tied to ground so the module is allowed to send. MOD is driven by P1.6: high selects command mode for the `ATZ` reset, and low selects data mode.

## Analysis and Results

The numbers below are computed from the firmware settings. Timer A0 compare values are the duty commands written by the motor driver. They are not a measured wheel speed. A labeled capture of P9.7 is made on the bench oscilloscope. 


**UART timing**

EUSCI_A3 uses SMCLK at 12 MHz. The baud-rate divider is an integer:

```text
N = 12,000,000 / 9600 = 1250
Bit period = 1 / 9600 = 104.2 µs
```

`UCMSB` in `UCAxCTLW0` is cleared, so each frame is LSB first: one start bit (low), D0 through D7, then one stop bit (high). The line idles high. A 100 ms gap follows every byte in the scope tests so the frames do not run together.

**Task 6.1 Part A — fixed bytes**

`UART_Transmit_Fixed_Pattern()` sends these four bytes, in order, through `BLE_UART_OutChar`:

| Byte | Binary | Order on P9.7 after the start bit |
|---|---|---|
| `0xAA` | `1010_1010` | 0, 1, 0, 1, 0, 1, 0, 1 |
| `0x55` | `0101_0101` | 1, 0, 1, 0, 1, 0, 1, 0 |
| `0xFF` | `1111_1111` | 1, 1, 1, 1, 1, 1, 1, 1 |
| `0x00` | `0000_0000` | 0, 0, 0, 0, 0, 0, 0, 0 |

`0x55` is the easiest frame to check. The start bit is low, then the data bits alternate beginning with 1, and the stop bit returns the line high:

```text
Idle  Start D0 D1 D2 D3 D4 D5 D6 D7 Stop  Idle
  1     0    1  0  1  0  1  0  1  0   1     1
        |<-------- 104.2 µs per bit -------->|
```

**output of the fixed bits on the oscilloscope**

**Task 6.1 Part B — PMOD SWT**

`PMOD_SWT_Init()` makes P10.0–P10.3 inputs. Pull-downs are enabled in `main` so an open switch reads as off. Each pass of the loop sends exactly one byte:

| Active switch | All others | Status bits | Byte |
|---|---|---|---|
| SW1 | Off | `0x01` | `0xAA` |
| SW2 | Off | `0x02` | `0x10` |
| SW3 | Off | `0x04` | `0xF0` |
| SW4 | Off | `0x08` | `0x0F` |
| Any other combination | — | other | `0x00` |

**SW1 only, byte `0xAA`**

![Oscilloscope capture for SW1, transmitting 0xAA](ece528L_ble_uart/BLE_UART/src/switch1.jpg)

**SW2 only, byte `0x10`**

![Oscilloscope capture for SW2, transmitting 0x10](ece528L_ble_uart/BLE_UART/src/switch2.jpg)

**SW3 only, byte `0xF0`**

![Oscilloscope capture for SW3, transmitting 0xF0](ece528L_ble_uart/BLE_UART/src/switch3.jpg)

**SW4 only, byte `0x0F`**

![Oscilloscope capture for SW4, transmitting 0x0F](ece528L_ble_uart/BLE_UART/src/switch4.jpg)

**Any other combination, byte `0x00`**

![Oscilloscope capture for any other switch combination, transmitting 0x00](ece528L_ble_uart/BLE_UART/src/switchmisc.jpg)

**Text commands**

A line ending in a line feed is printed on the Application UART terminal and then checked with `strstr`:

| Received string | Result |
|---|---|
| `RGB LED GREEN` | RGB LED green (P2.1) |
| `RGB LED OFF` | RGB LED off |
| Any other line | Terminal prints `BLE UART Command Not Found` |

**Task 6.2 — arrow-button motor control**

The Control Pad sends a 5-byte packet with no line ending. Byte 2 and byte 3 are ASCII characters. The checksum is read and discarded so it is not treated as the next header.

| Byte | Field | Value |
|---|---|---|
| 0 | Header | `!` (`0x21`) |
| 1 | Type | `B` (`0x42`) |
| 2 | Button | `5` up, `6` down, `7` left, `8` right |
| 3 | State | `1` pressed, `0` released |
| 4 | Checksum | Sum of the previous bytes, inverted |

Timer A0’s period constant is 15000, which is a 20 ms PWM period. Thirty percent of that period is the compare value used for every arrow press:

```text
Duty command = 0.30 × 15000 = 4500
```

| Button | Pressed | Released |
|---|---|---|
| Up (`5`) | `Motor_Forward(4500, 4500)` | `Motor_Stop()` |
| Down (`6`) | `Motor_Backward(4500, 4500)` | `Motor_Stop()` |
| Left (`7`) | `Motor_Left(4500, 4500)` | `Motor_Stop()` |
| Right (`8`) | `Motor_Right(4500, 4500)` | `Motor_Stop()` |

A press starts motion on that packet. A release stops both motors on that packet. The packet path does not call `Clock_Delay1ms`.

## Known Issues or Limitations

- **Text commands and button packets share one blocking read.** `BLE_UART_InChar()` waits until a byte arrives. If a button packet shows up while the loop is collecting a text line, those bytes are stored until a line feed arrives instead of being decoded as a motor command.
- **The button checksum is not checked.** A corrupted button number or state is still passed to the motor functions. The lab allows skipping the check, and the code still reads the fifth byte so the next packet can start on `!`.
- **One release stops the robot.** Each packet describes a single button. Releasing any arrow calls `Motor_Stop()`, including the case where another arrow was pressed earlier and is still held.
- **Only two text commands are recognized.** `RGB LED GREEN` and `RGB LED OFF` change the LED. Any other string, including other colors, prints `BLE UART Command Not Found` and leaves the motors alone.
- **Scope modes do not drive the robot.** `LAB_MODE` 1 and `LAB_MODE` 2 stay in the transmit path. Arrow control runs only when `LAB_MODE` is 0.
- **Open PMOD SWT inputs need the pull-downs added in `main`.** `PMOD_SWT_Init()` selects GPIO inputs and does not enable the resistors. Without the pull-downs, an off switch can read as on and select the wrong byte.
- **`BLE_UART_Reset()` blocks for several seconds.** It holds MOD high, waits 1 s, sends `ATZ`, waits 3 s, then returns MOD low. The main loop does not run during that time.

## Author Contributions

**Feranmi**
- Co-implemented all functions
- Recorded oscilloscope outputs
- Verified program function by flashing and running the program on the MSP432
- Co-authored README

**Brendan**
- Co-implemented all functions
- Created connection points for oscilloscope
- Co-authored README

### References

1. Microchip, *Introduction to Bluetooth Low Energy*.
2. Adafruit, *Introduction to Bluetooth Low Energy*.
3. Texas Instruments, *MSP432P4xx Technical Reference Manual* (SLAU356I), Section 12.2.6 (Function Select Registers `PxSEL0`, `PxSEL1`) and Section 24.4 (eUSCI_A UART Registers, `UCAxCTLW0` bit `UCMSB`).
4. Texas Instruments, *MSP432P401R Datasheet*, Table 6-45 (Default Mapping), Section 4.1 (Pin Diagrams), and Section 6.12.2 (Port P2, P2.0 to P2.3).
