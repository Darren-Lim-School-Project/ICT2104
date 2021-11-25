/*
 * PWM.h
 *
 *  Created on: 18 Oct 2021
 *      Author: darre
 */

#ifndef PWM_H_
#define PWM_H_

//void init_wheel(uint16_t leftWheelForward, uint16_t leftWheelBackward, uint16_t rightWheelForward, uint16_t rightWheelBackward);
void init_PWM(void);
void pwmLoop(void);
void slowDown(void);
void speedUp(void);
void leftDirection(void);
void rightDirection(void);
void forwardDirection(void);
void reverseDirection(void);
bool getSlowSpeed(void);

#endif /* PWM_H_ */
