#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

#define PWM_MOTOR_PERIOD 3599

void PWM_Init(void);
void PWM_SetCompare2(uint16_t Compare);
void PWM_SetCompare3(uint16_t Compare);
void PWM_SetCompare4(uint16_t Compare);
uint16_t PWM_GetPeriod(void);

#endif
