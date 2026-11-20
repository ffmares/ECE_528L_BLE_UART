/**
 * @file main.c
 *
 * @brief Main source code for the BLE_UART program.
 *
 * This file contains the main entry point for the BLE_UART program,
 * which is used to demonstrate the BLE_UART driver.
 *
 * It interfaces with the Adafruit Bluefruit LE UART Friend Bluetooth Low Energy (BLE) module, which uses the UART communication protocol.
 *  - Product Link: https://www.adafruit.com/product/2479
 *
 * @note For more information regarding the Enhanced Universal Serial Communication Interface (eUSCI),
 * refer to the MSP432Pxx Microcontrollers Technical Reference Manual
 *
 * @author Feranmi F., Brendan T.
 *
 */

#include "msp.h"
#include <stdint.h>
#include <stdio.h>

#include "inc/Clock.h"
#include "inc/CortexM.h"
#include "inc/GPIO.h"
#include "inc/EUSCI_A0_UART.h"
#include "inc/Motor.h"
#include "inc/BLE_UART.h"

// Select a lab exercise here, or define LAB_MODE in the compiler settings.
#define LAB_LED_DEMO       0
#define LAB_FIXED_BYTES    1
#define LAB_SWITCH_BYTES   2
#define LAB_CONTROLLER     3
#ifndef LAB_MODE
#define LAB_MODE LAB_FIXED_BYTES
#endif
#if LAB_MODE < LAB_LED_DEMO || LAB_MODE > LAB_CONTROLLER
#error Invalid LAB_MODE
#endif

#define PWM_NOMINAL ((TIMER_A0_PERIOD_CONSTANT * 30U) / 100U)

/** @brief Execute the two text commands from the Section 5 LED demonstration. */
void Process_BLE_UART_Data(char BLE_UART_Buffer[]){
    if (Check_BLE_UART_Data(BLE_UART_Buffer, "RGB LED GREEN")){
        LED2_Output(RGB_LED_GREEN);
    }
    else if (Check_BLE_UART_Data(BLE_UART_Buffer, "RGB LED OFF")){
        LED2_Output(RGB_LED_OFF);
    }
    else {
        printf("BLE UART Command Not Found\n");
    }
}
/**
 * @brief Decode a complete button packet and apply the Section 6.2 motor action.
 * @param buffer Five bytes returned by BLE_UART_InPacket, including checksum.
 * Non-button packets and invalid button/state fields are ignored.
 * Any arrow release stops both motors; processing contains no blocking delays.
 */
void Decode_Controller_Packet(const char buffer[])
{
    if (buffer[0] != '!' || buffer[1] != 'B') return;
    if (buffer[2] < '5' || buffer[2] > '8') return;
    if (buffer[3] == '0')
    {
        Motor_Stop();
        return;
    }
    if (buffer[3] != '1') return;

    switch (buffer[2])
    {
        case '5': Motor_Forward(PWM_NOMINAL, PWM_NOMINAL); break;
        case '6': Motor_Backward(PWM_NOMINAL, PWM_NOMINAL); break;
        case '7': Motor_Left(PWM_NOMINAL, PWM_NOMINAL); break;
        case '8': Motor_Right(PWM_NOMINAL, PWM_NOMINAL); break;
    }
}

/**
 * @brief Map the four switch inputs to the UART byte required by Task 6.1B.
 * @param switches SW1 through SW4 in bits 0 through 3.
 * @return Selected byte, or 0x00 for any other switch combination.
 */
uint8_t UART_Byte_For_Switches(uint8_t switches)
{
    switch (switches)
    {
        case 0x01: return 0xAA;
        case 0x02: return 0x10;
        case 0x04: return 0xF0;
        case 0x08: return 0x0F;
        default: return 0x00;
    }
}

/** @brief Initialize the hardware and run the selected lab exercise. */
int main(void)
{
    DisableInterrupts();
    Clock_Init48MHz();
    LED2_Init();
    EUSCI_A0_UART_Init_Printf();
    BLE_UART_Init();
    Motor_Init();
#if LAB_MODE == LAB_SWITCH_BYTES
    PMOD_SWT_Init();
#endif
    EnableInterrupts();

#if LAB_MODE == LAB_LED_DEMO || LAB_MODE == LAB_CONTROLLER
    char BLE_UART_Buffer[BLE_UART_BUFFER_SIZE] = {0};
    Clock_Delay1ms(1000);
    BLE_UART_Reset();
    BLE_UART_OutString("BLE UART Active\r\n");
    Clock_Delay1ms(1000);
#endif

    while (1)
    {
#if LAB_MODE == LAB_LED_DEMO
        int length = BLE_UART_InString(BLE_UART_Buffer, sizeof BLE_UART_Buffer);
        if (length < 0)
        {
            printf("BLE UART line too long\r\n");
            continue;
        }
        printf("BLE UART Data: %s\r\n", BLE_UART_Buffer);
        Process_BLE_UART_Data(BLE_UART_Buffer);
#elif LAB_MODE == LAB_FIXED_BYTES
        static const uint8_t bytes[] = {0xAA, 0x55, 0xFF, 0x00};
        unsigned int i;
        for (i = 0; i < sizeof bytes; ++i)
        {
            BLE_UART_OutChar(bytes[i]);
            Clock_Delay1ms(100);
        }
#elif LAB_MODE == LAB_SWITCH_BYTES
        BLE_UART_OutChar(UART_Byte_For_Switches(Get_PMOD_SWT_Status()));
        Clock_Delay1ms(100);
#elif LAB_MODE == LAB_CONTROLLER
        if (BLE_UART_InPacket(BLE_UART_Buffer, sizeof BLE_UART_Buffer) == BLE_UART_PACKET_SIZE)
        {
            Decode_Controller_Packet(BLE_UART_Buffer);
        }
#endif
    }
}
