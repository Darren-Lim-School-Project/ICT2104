#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <Devices/ESP8266.h>
#include <Hardware/UART_Driver.h>
#include <string.h>
#include <PWM.h>

/*We are connecting to api.themoviedb.org, ESP8266 can resolve DNS, pretty cool huh!*/
char HTTP_WebPage[] = "192.168.1.101";
/*HTTP is TCP port 80*/
char Port[] = "8000";
/*Data that will be sent to the HTTP server. This will allow us to query movie data. Get an api key from api.themoviedb.org*/
char HTTP_Request[] = "GET /test.php?name=abc&age=2 HTTP/1.0\r\n\r\n";
/*Subtract one to account for the null character*/
uint32_t HTTP_Request_Size = sizeof(HTTP_Request) - 1;

const char *ssid = "IRAcer";
const char *password = "123321";
char* receivedData;

eUSCI_UART_ConfigV1 UART0Config = {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK, 1, 10, 0,
        EUSCI_A_UART_NO_PARITY,
        EUSCI_A_UART_LSB_FIRST,
        EUSCI_A_UART_ONE_STOP_BIT,
        EUSCI_A_UART_MODE,
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION };

eUSCI_UART_ConfigV1 UART2Config = {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK, 1, 10, 0,
        EUSCI_A_UART_NO_PARITY,
        EUSCI_A_UART_LSB_FIRST,
        EUSCI_A_UART_ONE_STOP_BIT,
        EUSCI_A_UART_MODE,
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION };

static void Delay(uint32_t loop)
{
    volatile uint32_t i;

    for (i = 0; i < loop; i++)
        ;
}

void init_UART()
{
    MAP_WDT_A_holdTimer();

    /*Ensure MSP432 is Running at 24 MHz*/
    //FlashCtl_setWaitState(FLASH_BANK0, 2);
    //FlashCtl_setWaitState(FLASH_BANK1, 2);
    //PCM_setCoreVoltageLevel(PCM_VCORE1);
    int a = CS_getSMCLK();
    //printf("UART SMCLK: %d\n", a);

    //CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_24);

    /*Initialize required hardware peripherals for the ESP8266*/
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_UART_initModule(EUSCI_A0_BASE, &UART0Config);
    MAP_UART_enableModule(EUSCI_A0_BASE);
    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA0);

    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_UART_initModule(EUSCI_A2_BASE, &UART2Config);
    MAP_UART_enableModule(EUSCI_A2_BASE);
    MAP_UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA2);

    /*Reset GPIO of the ESP8266*/
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN1);

    MAP_Interrupt_enableMaster();

    /*Hard Reset ESP8266*/
    ESP8266_HardReset();
    __delay_cycles(48000000);
    UART_Flush(EUSCI_A2_BASE);
    //ESP8266_Disconnect();

    char *ESP8266_Data = ESP8266_GetBuffer();
    receivedData = ESP8266_Data;
    char keyword = "l";

    /*
     if (!ESP8266_GetIP()) {
     MSPrintf(EUSCI_A0_BASE, ESP8266_Data);
     }
     */

    //MSPrintf(EUSCI_A0_BASE, "Get IP: %s\r\n", ESP8266_Data);
    if (!ESP8266_ChangeMode1())
    {
        MSPrintf(EUSCI_A0_BASE, ESP8266_Data);
    }

    MSPrintf(EUSCI_A0_BASE, "Mode changed to 1\r\n");

    if (!ESP8266_EnableMultipleConnections(true))
    {
        MSPrintf(EUSCI_A0_BASE,
                 "Unable to connect establish multiple connection\n");
    }

    MSPrintf(EUSCI_A0_BASE, "Multiple connection enabled\r\n");

    if (!ESP8266_Port80()) //keep trying to connect
    {
        MSPrintf(EUSCI_A0_BASE, ESP8266_Data);
    }

    MSPrintf(EUSCI_A0_BASE, "Turned on port 8080\r\n");

    if (!ESP8266_ConnectToAP("KimVeryCute", "money888")) //keep trying to connect
         {
         MSPrintf(EUSCI_A0_BASE, ESP8266_Data);
         }

         MSPrintf(EUSCI_A0_BASE, "Successfully connect to WiFi\r\n"); //connected
         __delay_cycles(48000000);

    //ESP8266_Terminal();

    /*
     if (!ESP8266_ConnectToAP("Lim", "weiloowoov")) //keep trying to connect
     {
     MSPrintf(EUSCI_A0_BASE, ESP8266_Data);
     }

     MSPrintf(EUSCI_A0_BASE, "Successfully connect to WiFi\r\n"); //connected
     __delay_cycles(48000000);
     */

    /*
     if(!ESP8266_EstablishConnection('0', TCP, HTTP_WebPage, Port)) {
     MSPrintf(EUSCI_A0_BASE, "Failed to connect to: %s\r\nERROR: %s\r\n", HTTP_WebPage, ESP8266_Data);
     while(1);
     }

     MSPrintf(EUSCI_A0_BASE, "Connected to: %s\r\n\r\n", HTTP_WebPage);

     if(!ESP8266_SendData('0', HTTP_Request, HTTP_Request_Size))
     {
     MSPrintf(EUSCI_A0_BASE, "Failed to send: %s to %s \r\nError: %s\r\n", HTTP_Request, HTTP_WebPage, ESP8266_Data);
     while(1);
     }

     MSPrintf(EUSCI_A0_BASE, "Data sent: %s to %s\r\n\r\nESP8266 Data Received: %s\r\n", HTTP_Request, HTTP_WebPage, ESP8266_Data);
     */

    /*Start ESP8266 serial terminal, will not return*/

    /*
     while (1)
     {
     if (ESP8266_WaitForAnswer(1))
     {
     MSPrintf(EUSCI_A0_BASE, "Data Received: %s\n", ESP8266_Data);

     if (strstr(ESP8266_GetBuffer(), "pin=") != NULL)
     {
     char holder = ESP8266_Data[34];

     switch (ESP8266_Data[34]) {
     case 'l':
     leftDirection();
     break;
     case 'r':
     rightDirection();
     break;
     case 'f':
     forwardDirection();
     break;
     case 'b':
     reverseDirection();
     break;
     }

     if (strcmp(ESP8266_Data[18], '0') == 0)
     {
     //printf("0 detected\n");
     if (ESP8266_Close(ESP8266_Data[18]))
     {
     MSPrintf(EUSCI_A0_BASE, ESP8266_Data);
     }
     }

     ESP8266_ClearBuffer();
     }
     }
     }

     ESP8266_Terminal();
     */
}

void uartLoop(void)
{
    if (ESP8266_WaitForAnswer(1))
    {
        MSPrintf(EUSCI_A0_BASE, "Data Received: %s\n", receivedData);

        if (strstr(ESP8266_GetBuffer(), "pin=") != NULL)
        {
            char holder = receivedData[34];

            switch (receivedData[34])
            {
            case 'l':
                leftDirection();
                break;
            case 'r':
                rightDirection();
                break;
            case 'f':
                forwardDirection();
                break;
            case 'b':
                reverseDirection();
                break;
            }

            if (strcmp(receivedData[18], '0') == 0)
            {
                //printf("0 detected\n");
                if (ESP8266_Close(receivedData[18]))
                {
                    MSPrintf(EUSCI_A0_BASE, receivedData);
                }
            }

            ESP8266_ClearBuffer();
        }
    }
}
