#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "Ultrasonic.h"
#include <math.h>
#define TICKPERIOD      1000
extern int rightrotation;
extern int leftrotation;
extern const float diameter;

uint32_t SR04IntTimes;
uint32_t encoderTimes;

// -------------------------------------------------------------------------------------------------------------------

static void Delay(uint32_t loop)
{
    volatile uint32_t i;

    for (i = 0 ; i < loop ; i++);
}

// -------------------------------------------------------------------------------------------------------------------

void init_ultrasonic(void)
{
    /* Timer_A UpMode Configuration Parameter */
    const Timer_A_UpModeConfig upConfig =
    {
            TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
            TIMER_A_CLOCKSOURCE_DIVIDER_3,          // SMCLK/3 = 1MHz
            TICKPERIOD,                             // 1000 tick period
            TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
            TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
            TIMER_A_DO_CLEAR                        // Clear value
    };

    int a = CS_getSMCLK();

    /* Configuring P3.6 as Output */
    //P3.6 connected to Trig of HCSR04 (Yellow Wire)
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6);//
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);//
    //P3.7 connected to Echo of HCSR04 (Green Wire)
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN7);


    /* Configuring Timer_A0 for Up Mode */
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);

    /* Enabling interrupts and starting the timer */
    Interrupt_enableInterrupt(INT_TA1_0);
    //Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    //Timer_A_stopTimer(TIMER_A0_BASE);
    Timer_A_clearTimer(TIMER_A1_BASE);

}

// -------------------------------------------------------------------------------------------------------------------

void TA1_0_IRQHandler(void)
{
    /* Increment global variable (count number of interrupt occurred) */
    SR04IntTimes++;
    encoderTimes++;


    if (encoderTimes == 1000) {
        float rightrpm = ((float)rightrotation/20) *60;
        float leftrpm = ((float)leftrotation/20) *60;

        rightrotation=0;
        leftrotation=0;
        float rightspeed = rightrpm * (diameter/2.54) * M_PI * ((float)60/63360);
        //float leftspeed = leftrpm * (diameter/2.54) * M_PI * ((float)60/63360);
        //float distance = ((rightrpm+leftrpm)/2) * (diameter/2.54) * M_PI;
        float distance = (rightrpm) * (diameter/2.54) * M_PI;

        encoderTimes=0;
    }

    /* Clear interrupt flag */
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
}

// -------------------------------------------------------------------------------------------------------------------

static uint32_t getHCSR04Time(void)
{
    uint32_t pulsetime=0;

    /* Number of times the interrupt occurred (1 interrupt = 1000 ticks)    */
    pulsetime = SR04IntTimes * TICKPERIOD;

    /* Number of ticks (between 1 to 999) before the interrupt could occur */
    pulsetime += Timer_A_getCounterValue(TIMER_A1_BASE);

    /* Clear Timer */
    Timer_A_clearTimer(TIMER_A1_BASE);

    Delay(3000);

    return pulsetime;
}

// -------------------------------------------------------------------------------------------------------------------

float getHCSR04Distance(void)
{
    uint32_t pulseduration = 0;
    float calculateddistance = 0;

    /* Generate 10us pulse at P3.6 */
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6);
    Delay(30);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);

    //GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Wait for positive-edge */
    while(GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN7) == 0);
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0);

    /* Start Timer */
    SR04IntTimes = 0;
    Timer_A_clearTimer(TIMER_A1_BASE);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    /* Detects negative-edge */
    while(GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN7) == 1);

    /* Stop Timer */
    Timer_A_stopTimer(TIMER_A1_BASE);

    /* Obtain Pulse Width in microseconds */
    pulseduration = getHCSR04Time();

    /* Calculating distance in cm */
    calculateddistance = (float)pulseduration / 58.0f;

    return calculateddistance;
}
