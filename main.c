/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 Empty Project
 *
 * Description: An empty project that uses DriverLib
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 * Author: 
 *******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* Importing files */
#include "PWM.h"
#include "UART.h"

// Global Variable for Wheel. 20 = 1 round.
volatile static uint32_t counter;
//bool test = false;

inline void uart_println(const char *str, ...)
{
    static char print_buffer[256];
    va_list lst;
    va_start(lst, str);
    vsnprintf(print_buffer, 256, str, lst);
    str = print_buffer;
    while (*str)
    {
        while (!(UCA0IFG & EUSCI_A_IFG_TXIFG))
            ;
        UCA0TXBUF = *str;
        str++;
    }
    while (!(UCA0IFG & EUSCI_A_IFG_TXIFG))
        ;
    UCA0TXBUF = '\r';
    while (!(UCA0IFG & EUSCI_A_IFG_TXIFG))
        ;
    UCA0TXBUF = '\n';
}

int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Disable watchdog

    init_wheel(BIT0, BIT1, BIT2, BIT3);  // Output for Wheels
    //init_PWM(BIT2, BIT4);   // Output for PWMs
    //setUpPWM();

    counter = 0;
    setUpUART();

    /////* for uart_println */////
    // init UART (LSB first, 1 stop bit, no parity, 8-bit characters, 115200 baud)
    P1SEL0 = 0x0c;
    UCA0CTLW0 = UCSWRST | EUSCI_A_CTLW0_SSEL__SMCLK;
    UCA0BRW = (uint16_t) ((uint32_t) 3000000 / 115200);
    UCA0MCTLW = 0;
    UCA0CTLW0 &= ~UCSWRST;
    /////* for uart_println */////

    // P2.5 Input Pin
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN5); // Set P1.1 as input (Switch 1)

    // Setting Interrupt to trigger only at Rising Edge.
    GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1,
                             GPIO_LOW_TO_HIGH_TRANSITION);

    // P1.0 Output Pin
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    // Clear Interrupt Flag Pin 2.5
    GPIO_clearInterruptFlag(GPIO_PORT_P2, GPIO_PIN5);

    // Enabling Interrupt for Pin 2.5
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN5);

    // Enable Interrupt for Port 2
    Interrupt_enableInterrupt(INT_PORT2);

    // Enable Master Interrupt
    Interrupt_enableMaster();

    // Forever Loop the Low Power Mode State 3
    while (1)
    {
        PCM_gotoLPM0();
        PCM_gotoLPM3();
    }
}

// Port 2 ISR
void PORT2_IRQHandler(void)
{
    // Local Variable to store status of Interrupt
    int local;

    // Storing state of interrupt flag onto local variable
    local = P2->IFG;

    // Increment Global Count Variable
    counter++;
    if (counter == 20)
    {
        uart_println("ASD");
        // Toggle Pin
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        counter = 0;
        local = 0;
    }

    // Clearing Interrupt Flag
    GPIO_clearInterruptFlag(GPIO_PORT_P2, GPIO_PIN5);
}
