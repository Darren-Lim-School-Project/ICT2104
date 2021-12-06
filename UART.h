/*
 * UART.h
 *
 *  Created on: 18 Oct 2021
 *      Author: darre
 */

#ifndef UART_H_
#define UART_H_

static void Delay(uint32_t loop);
void uartLoop(void);
init_UART();
void sendData(int typesOfData, float value);


#endif /* UART_H_ */
