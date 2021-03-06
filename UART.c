#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <Devices/ESP8266.h>
#include <Hardware/UART_Driver.h>
#include <string.h>
#include <PWM.h>

// IP Address of Web Server in the same network
char HTTP_WebPage[] = "172.20.10.3";

// Port 8000 will be used
char Port[] = "8000";

// Data will be sent to this path of the Web Server
char HTTP_Request[] = "GET /src/mvc/view/receiver.php?speed=123 HTTP/1.0\r\n\r\n";

/*Subtract one to account for the null character*/
uint32_t HTTP_Request_Size = sizeof(HTTP_Request) - 1;

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
    UART_Flush(EUSCI_A0_BASE);
    UART_Flush(EUSCI_A2_BASE);
    //ESP8266_Disconnect();

    char *ESP8266_Data = ESP8266_GetBuffer();
    receivedData = ESP8266_Data;

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

    if (!ESP8266_Port8000())
    {
        MSPrintf(EUSCI_A0_BASE, ESP8266_Data);
    }
    UART_Write(EUSCI_A2_BASE,"AT+CIPSTO=3600\r\n",19);
    MSPrintf(EUSCI_A0_BASE, "Turned on port 8000\r\n");

    if (!ESP8266_ConnectToAP("Darren", "abcd1234")) //keep trying to connect
    {
    MSPrintf(EUSCI_A0_BASE, ESP8266_Data);
    }

    MSPrintf(EUSCI_A0_BASE, "Successfully connect to WiFi\r\n"); //connected

    //ESP8266_Terminal();

    /*
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
}

/*
 *
 * Send data over to Web Server based on the input IP Address and Web Path through Connection ID 1
 *
 */
void sendData(int typesOfData, float value) {

    switch(typesOfData) {
    case 1:
        if(!ESP8266_EstablishConnection('1', TCP, HTTP_WebPage, Port)) {
             MSPrintf(EUSCI_A0_BASE, "Failed to connect to: %s\r\nERROR: %s\r\n", HTTP_WebPage, ESP8266_GetBuffer());
             }
        MSPrintf(EUSCI_A0_BASE, "Connected to: %s\r\n\r\n", HTTP_WebPage);
        if(!ESP8266_SendData('1', HTTP_Request, HTTP_Request_Size))
             {
             MSPrintf(EUSCI_A0_BASE, "Failed to send: %s to %s \r\nError: %s\r\n", HTTP_Request, HTTP_WebPage, ESP8266_GetBuffer());
             //while(1);
             }

        MSPrintf(EUSCI_A0_BASE, "Data sent: %s to %s\r\n\r\nESP8266 Data Received: %s\r\n", HTTP_Request, HTTP_WebPage, ESP8266_GetBuffer());
        break;
    }

}

/*
 *
 * Function used for main, whereby it will check if there are any data that are being sent to
 * ESP8266. It will check on the data if there are string "pin=", whereby it is part of the data sent
 * from Web Server to ESP8266. It will then be processed based on the received data
 *
 */
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
                if (ESP8266_Close(receivedData[18]))
                {
                    MSPrintf(EUSCI_A0_BASE, receivedData);
                }
            }

            ESP8266_ClearBuffer();
        }
    }
}
