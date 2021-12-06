#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

/* Statics */
#define TICKPERIOD      1000
int rightrotation =0;
int leftrotation =0;
const float diameter = 6.61; //wheel diameter in cm

const Timer_A_UpModeConfig upConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_3,          // SMCLK/3 = 1MHz
        TICKPERIOD,                             // 62500 tick period (1second)
        TIMER_A_TAIE_INTERRUPT_DISABLE,      // Enable Overflow ISR
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Disable CCR0 interrupt
        TIMER_A_DO_CLEAR                    // Clear Counter
};

void init_Encoder()
{
    /* Stop watchdog timer */
    MAP_WDT_A_holdTimer();
    /*right wheel encoder*/
    GPIO_clearInterruptFlag(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN5);
    /*left wheel encoder*/
    GPIO_clearInterruptFlag(GPIO_PORT_P2, GPIO_PIN4);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN4);

    /* Configuring Up Mode */
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);

    /* Enabling interrupts */
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN4);
    Interrupt_enableInterrupt(INT_PORT2);
    Interrupt_enableInterrupt(INT_TA1_0);

    /* Enabling MASTER interrupts */
    Interrupt_enableMaster();

    /* Starting the Timer_A0 in up mode */
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

}

/* Port 2 ISR */
void PORT2_IRQHandler(void)
{
    if (GPIO_getInterruptStatus(GPIO_PORT_P2, GPIO_PIN5)){
        //printf("Port2 Pin 5 Interrupt\n");
        rightrotation++;
        GPIO_clearInterruptFlag(GPIO_PORT_P2, GPIO_PIN5);
    } else if (GPIO_getInterruptStatus(GPIO_PORT_P2, GPIO_PIN4)) {
        //printf("Port2 Pin 4 Interrupt\n");
        leftrotation++;
        GPIO_clearInterruptFlag(GPIO_PORT_P2, GPIO_PIN4);
    }
}

