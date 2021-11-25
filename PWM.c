/*
 * PWM.c
 *
 *  Created on: 18 Oct 2021
 *      Author: darren
 */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdbool.h>
#include <PWM.h>

/* Timer_A PWM Configuration Parameter */
Timer_A_PWMConfig pwmConfig = {
TIMER_A_CLOCKSOURCE_SMCLK,
                                TIMER_A_CLOCKSOURCE_DIVIDER_24, 10000,
                                TIMER_A_CAPTURECOMPARE_REGISTER_3,
                                TIMER_A_OUTPUTMODE_RESET_SET, 0 };

Timer_A_PWMConfig pwm2Config = {
TIMER_A_CLOCKSOURCE_SMCLK,
                                 TIMER_A_CLOCKSOURCE_DIVIDER_24, 10000,
                                 TIMER_A_CAPTURECOMPARE_REGISTER_4,
                                 TIMER_A_OUTPUTMODE_RESET_SET, 0 };

static void Delay(uint32_t loop)
{
    volatile uint32_t i;

    for (i = 0 ; i < loop ; i++);
}

void init_PWM(void)
{

    /* Configuring P4.4 and P4.5 as Output. P2.4 as peripheral output for PWM and P1.1 for button interrupt */
    GPIO_setAsOutputPin(GPIO_PORT_P4,
    GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3);

    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1 | GPIO_PIN2);

    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,
    GPIO_PIN6 | GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);
    /* Configuring Timer_A to have a period of approximately 80ms and an initial duty cycle of 10% of that (1000 ticks)  */
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwm2Config);
}

/* Port1 ISR - This ISR will progressively step up the duty cycle of the PWM on a button press */
void slowDown(void)
{
    pwmConfig.dutyCycle = 2000;
    pwm2Config.dutyCycle = 2000;
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwm2Config);
}

void speedUp(void)
{
    pwmConfig.dutyCycle = 9000;
    pwm2Config.dutyCycle = 9000;
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwm2Config);
}

void leftDirection(void)
{
    pwmConfig.dutyCycle = 9000;
    pwm2Config.dutyCycle = 3000;
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwm2Config);
    Delay(1000000);
    forwardDirection();
}

void rightDirection(void)
{
    pwmConfig.dutyCycle = 3000;
    pwm2Config.dutyCycle = 9000;
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwm2Config);
    Delay(1000000);
    forwardDirection();
}

void forwardDirection(void) {
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1 | GPIO_PIN2);
    speedUp();
}

void reverseDirection(void)
{
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1 | GPIO_PIN2);
    speedUp();
}

bool getSlowSpeed(void) {
    //printf("dutyCycle: %d\n", pwmConfig.dutyCycle);
    if (pwmConfig.dutyCycle == 2000 && pwm2Config.dutyCycle == 2000) {
        return true;
    } else {
        return false;
    }
}
