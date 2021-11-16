/*
 * PWM.c
 *
 *  Created on: 18 Oct 2021
 *      Author: darren
 */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
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

/*
 void init_wheel(uint16_t rightWheelBackward, uint16_t rightWheelForward,
 uint16_t leftWheelForward, uint16_t leftWheelBackward)
 {
 P4DIR |= rightWheelBackward + rightWheelForward + leftWheelForward
 + leftWheelBackward; // Output for the 4 pins
 P2DIR |= BIT4+ BIT6 + BIT7;

 //P4OUT |= leftWheelForward;
 //P4OUT &= ~leftWheelBackward;]
 P4OUT &= ~(rightWheelBackward + rightWheelForward + leftWheelForward
 + leftWheelBackward); // To stop all 4 wheels

 P4OUT |= rightWheelForward;

 }
 */

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
