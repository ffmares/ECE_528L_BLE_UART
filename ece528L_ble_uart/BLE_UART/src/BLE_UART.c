/**
 * @file BLE_UART.c
 *
 * @brief Source code for the BLE_UART driver.
 *
 * This file contains the function definitions for the BLE_UART driver.
 *
 * It interfaces with the Adafruit Bluefruit LE UART Friend Bluetooth Low Energy (BLE) module, which uses the UART communication protocol.
 *  - Product Link: https://www.adafruit.com/product/2479
 *
 * The following connections must be made:
 *  - BLE UART MOD  (Pin 1)     <-->  MSP432 LaunchPad Pin P1.6
 *  - BLE UART CTS  (Pin 2)     <-->  MSP432 LaunchPad GND
 *  - BLE UART TXO  (Pin 3)     <-->  MSP432 LaunchPad Pin P9.6 (PM_UCA3RXD)
 *  - BLE UART RXI  (Pin 4)     <-->  MSP432 LaunchPad Pin P9.7 (PM_UCA3TXD)
 *  - BLE UART VIN  (Pin 5)     <-->  MSP432 LaunchPad VCC (3.3V)
 *  - BLE UART RTS  (Pin 6)     <-->  Not Connected
 *  - BLE UART GND  (Pin 7)     <-->  MSP432 LaunchPad GND
 *  - BLE UART DFU  (Pin 8)     <-->  Not Connected
 *
 * @note For more information regarding the Enhanced Universal Serial Communication Interface (eUSCI),
 * refer to the MSP432Pxx Microcontrollers Technical Reference Manual
 *
 * @author
 *
 */

#include "../inc/BLE_UART.h"

void BLE_UART_Init()
{
    P9->SEL0 |= 0xC0;
    P9->SEL1 &= ~ (OxC0);

    // Configuring the P1.6 pin as an output GPIO pin
    // Clearing the Bit 6 of SEL0/1 registers
    P1->SEL0 &= ~0x40;
    P1->SEL1 &= ~0x40;
    P1->DIR |= 0x40;

    // Holding the EUSCI_A3 module in the reset state
    EUSCI_A3->CTLW0 |= 0x01;

    // Clear all of the bits in the Modulation Control Word (MCTLW) register
    EUSCI_A3->MCTLW &= ~(0xFF);

    // Disable the parity bit by clearing the UCPEN bit
    EUSCI_A3->CTLW0 &= ~(0x8000);

    // Select the odd parity bit by clearing UCPAR bit
    EUSCI_A3->CTLW0 &= ~(0x4000);

    // Set the bit order to LSB by clearing UCMSB bit (13)
    EUSCI_A3->CTLW0 &= ~(0x2000);

    // Select the 8-bit character length by clearing UC7BIT bit (12)
    EUSCI_A3->CTLW0 &= ~(0x1000);

    // Select one stop bit by clearing the UCSPB bit (11)
    EUSCI_A3->CTLW0 &= ~(0x0800);

    // Enable UART mode by writing 00b to UCMODEx (10 and 9)
    EUSCI_A3->CTLW0 &= ~(0x0600);

    // Disable synchronous mode by clearing the UCSYNC bit (8)
    EUSCI_A3->CTLW0 &= ~(0X0100);

    // Use SMCLK
    EUSCI_A3->CTLW0 |= 0X00C0;

    // Set the baud rate by writing to the UCBRx field (15 to 0)
    EUSCI_A3->BRW = 1250;

    // Disable the transmit complete interrupt UCTXCPTIE
    EUSCI_A3->IE &= ~(0x0C);

    // Enable the transmit interrupt UCTXIE and the receive interrupt UCRXIE
    EUSCI_A3->IE |= 0x03;

    // Release the EUSCI_A3 module from the reset state by clearing the UCSWRST bit (0)
    EUSCI_A3->CTLW0 &= ~(0x01);
}

uint8_t BLE_UART_InChar()
{
    while ((EUSCI_A3->IFG & 0x01) == 0);
    return EUSCI_A3->RXBUF;
}

void BLE_UART_OutChar(uint8_t data)
{
    while((EUSCI_A3->IFG & 0x02) == 0);
    EUSCI_A3->TXBUF = data;
}

int BLE_UART_InString(char *buffer_pointer, uint16_t buffer_size)
{
    int length = 0;
    int string_size = 0;

    // Read the last received data from the UART Receive Buffer
    char character = BLE_UART_InChar();

    while(character != LF){
        // Remove the character from the buffer is the received character is a backspace character
        if (character == BS) {
            if (length) {
                buffer_pointer--;
                length--;
                BLE_UART_OutChar(BS);
            }
        }
        else if (length < buffer_size){
            *buffer_pointer = character;
            buffer_pointer++;
            length++;
            string)size++;
        }
        character = BLE_UART_InChar();
    }
    *buffer_pointer = 0;
    return string_size;
    return 0;
}

void BLE_UART_OutString(char *pt)
{
    while (*pt){
        BLE_UART_OutChar(*pt);
        pt++;
    }
}

uint8_t Check_BLE_UART_Data(char BLE_UART_Data_Buffer[], char *data_string)
{
    if (strstr(BLE_UART_Data_Buffer, data_string) != NULL)
    {
        return 0x01;
    }

    else
    {
        return 0x00;
    }
}

void BLE_UART_Reset()
{
}
