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
#include "UART.h"
#include "IRLine.h"

#define SAFE_DISTANCE 20.0f
#define DANGER_DISTANCE 10.0f

// Global Variable for Wheel. 20 = 1 round.
volatile static uint32_t counter;
float foo;

static void Delay(uint32_t loop)
{
    volatile uint32_t i;

    for (i = 0; i < loop; i++)
        ;
}

/*
 * This is where everything starts
 * Initialize all the components and enter a while(1) loop to keep the car running
 */

int main(void)
{

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Disable watchdog
    int sendDataCounter = 0;

    // Status pin
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1 | GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN2);

    init_UART();
    init_ultrasonic();
    init_PWM();
    init_Encoder();
    IRSensorSetup();
    while (1)
    {
        // If any side of the IR line detector has a trigger 1, then it will stop the car
        if (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN1) == 1
                || GPIO_getInputPinValue(GPIO_PORT_P2, GPIO_PIN3) == 1)
        {
            Delay(100000);
            //GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
            //GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
            //GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
            stop();
            //GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
            //GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
            //GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
            //GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        }
        else
        {
            // Else it will enter this area to listen to any commands sent from web interface
            Delay(5000);
            uartLoop();

            // To delay the sending or data over to web after 200 counts of sendDataCounter
            sendDataCounter++;
            if (sendDataCounter == 200)
            {
                sendData(1, foo);
                sendDataCounter = 0;
            }

            // If distance to object is less than 10cm, the car will stop
            // Else if the distance is less than 20cm, the car will slow down
            // Else the car will resume at normal speed
            foo = getHCSR04Distance();
            if ((getHCSR04Distance() < DANGER_DISTANCE) && (!getSlowSpeed()))
            {
                stop();
                //GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
                Delay(50000);
            }
            else if ((getHCSR04Distance() < SAFE_DISTANCE) && (!getSlowSpeed()))
            {
                slowDown();
                //GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);
                Delay(50000);
            }
            else
            {
                speedUp();
                //GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);
                Delay(50000);
            }
        }

    }
}
