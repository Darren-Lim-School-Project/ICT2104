#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// Set both P5PIN1 and P2PIN3 as Input pin
void IRSensorSetup(void){
     GPIO_setAsInputPin(GPIO_PORT_P5, GPIO_PIN1); //Set P5.4 as input
     GPIO_setAsInputPin(GPIO_PORT_P2, GPIO_PIN3); //Set P5.4 as input
}
