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
#include "Ultrasonic.h"
//#include "UART.h"

#define MIN_DISTANCE 15.0f

// Global Variable for Wheel. 20 = 1 round.
volatile static uint32_t counter;

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

static void Delay(uint32_t loop)
{
    volatile uint32_t i;

    for (i = 0 ; i < loop ; i++);
}

int main(void)
{
    float foo;
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Disable watchdog

    //init_wheel(BIT0, BIT1, BIT2, BIT3);     // Initialize Wheels
    //printf("Wheels initialization completed\n");

    init_PWM();                 // Output for PWMs

    init_UART();                            // Initialize UART
    printf("UART initialization completed\r\n");

    init_ultrasonic();
    while (1)
    {
        Delay(100000);
        foo = getHCSR04Distance();
        /* Obtain distance from HCSR04 sensor and check if its less then minimum distance */
        if ((foo < MIN_DISTANCE)) {
            printf("NEAR: %d\n", (int) foo);
            //slowDown();
        }
        else {
            printf("FAR: %d\n", (int) foo);
            //speedUp();
        }
    }
}

// Port 2 ISR
/*
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
 */
