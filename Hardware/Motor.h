#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"

#define MOTOR_OUTPUT_MAX 1000

void Motor_Init(void);

void Motor_SetOutput(int16_t Output);

void Motor_Stop(void);

int16_t Motor_GetOutput(void);

#endif
