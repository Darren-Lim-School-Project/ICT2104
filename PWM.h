/*
 * PWM.h
 *
 *  Created on: 18 Oct 2021
 *      Author: darre
 */

#ifndef PWM_H_
#define PWM_H_

void init_wheel(uint16_t leftWheelForward, uint16_t leftWheelBackward, uint16_t rightWheelForward, uint16_t rightWheelBackward);
void setUpPWM();
void PORT1_IRQHandler(void);


#endif /* PWM_H_ */
