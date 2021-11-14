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
                                TIMER_A_CLOCKSOURCE_DIVIDER_24,
                                10000,
                                TIMER_A_CAPTURECOMPARE_REGISTER_1,
                                TIMER_A_OUTPUTMODE_RESET_SET,
                                1000};

Timer_A_PWMConfig pwmConfig_B = {
TIMER_A_CLOCKSOURCE_SMCLK,
                                  TIMER_A_CLOCKSOURCE_DIVIDER_24, 10000,
                                  TIMER_A_CAPTURECOMPARE_REGISTER_1,
                                  TIMER_A_OUTPUTMODE_RESET_SET, 1000 };

void init_wheel(uint16_t rightWheelBackward, uint16_t rightWheelForward,
                uint16_t leftWheelForward, uint16_t leftWheelBackward)
{
    P4DIR |= rightWheelBackward + rightWheelForward + leftWheelForward
            + leftWheelBackward; // Output for the 4 pins
    P2DIR |= BIT4 + BIT6;

    //P4OUT |= leftWheelForward;
    //P4OUT &= ~leftWheelBackward;]
    P4OUT &= ~(rightWheelBackward + rightWheelForward + leftWheelForward
            + leftWheelBackward); // To stop all 4 wheels

    P4OUT |= rightWheelForward;

}

void init_PWM(uint16_t ENA, uint16_t ENB)
{
    int a = CS_getSMCLK();
    printf("SMCLK: %d\n", a);
    // Start of PWM - Left Motor
    /* Configuring P4.0 and P4.1 as Output. P2.4 as peripheral output for PWM and P1.1 for button interrupt */
    //GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0);
    //GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN1);
    //GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0);
    //GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, ENA,
    GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);

    /* Configuring Timer_A to have a period of approximately 80ms and an initial duty cycle of 10% of that (1000 ticks)  */
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);

    /* Enabling interrupts and starting the watchdog timer */
    Interrupt_enableInterrupt(INT_PORT1);
    //Interrupt_enableSleepOnIsrExit();
    Interrupt_enableMaster();
    // End of PWM - Left Motor

    // Start of PWM - Right Motor
    /* Configuring P4.2 and P4.0 as Output. P2.6 as peripheral output for PWM and P1.4 for button interrupt */
    /*
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN6,
    GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);
    */

    /* Configuring Timer_A to have a period of approximately 80ms and an initial duty cycle of 10% of that (1000 ticks)  */
    /*
    Timer_A_generatePWM(TIMER_A1_BASE, &pwmConfig_B);
    */

    /* Enabling interrupts and starting the watchdog timer */
    /*
    Interrupt_enableInterrupt(INT_PORT1);
    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableMaster();
    // End of PWM - Right Motor
    */
}

/*
void PORT3_IRQHandler(void)
{
    printf("Port1 IRQ");
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if (status & GPIO_PIN0)
    {
        if (pwmConfig.dutyCycle == 9000) {
            printf("SMCLK: %d\n", CS_getSMCLK());
            printf("Duty Cycle: %d\n", pwmConfig.dutyCycle);
            pwmConfig.dutyCycle = 1000;
        } else {
            printf("SMCLK: %d\n", CS_getSMCLK());
            printf("Duty Cycle: %d\n", pwmConfig.dutyCycle);
            pwmConfig.dutyCycle += 1000;
        }
        */

        /*
        if (pwmConfig_B.dutyCycle == 9000)
            pwmConfig_B.dutyCycle = 1000;
        else
            pwmConfig_B.dutyCycle += 1000;
        */
/*
        Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
        //Timer_A_generatePWM(TIMER_A1_BASE, &pwmConfig_B);
    }
}
*/
