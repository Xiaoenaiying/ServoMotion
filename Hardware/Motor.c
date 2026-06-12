#include "Motor.h"
#include "PWM.h"

static int16_t Motor_Output = 0;

void Motor_Init(void) {
    PWM_Init();
    Motor_Stop();
}

void Motor_SetOutput(int16_t Output) {
    uint16_t Compare;

    if (Output > MOTOR_OUTPUT_MAX) Output = MOTOR_OUTPUT_MAX;
    if (Output < -MOTOR_OUTPUT_MAX) Output = -MOTOR_OUTPUT_MAX;

    Motor_Output = Output;

    if (Output > 0) {
        GPIO_SetBits(GPIOA, GPIO_Pin_2);
        GPIO_ResetBits(GPIOA, GPIO_Pin_3);
        Compare = (uint16_t) ((uint32_t) Output * PWM_GetPeriod() / MOTOR_OUTPUT_MAX);
        PWM_SetCompare2(Compare);
    } else if (Output < 0) {
        GPIO_ResetBits(GPIOA, GPIO_Pin_2);
        GPIO_SetBits(GPIOA, GPIO_Pin_3);
        Compare = (uint16_t) ((uint32_t) (-Output) * PWM_GetPeriod() / MOTOR_OUTPUT_MAX);
        PWM_SetCompare2(Compare);
    } else {
        Motor_Stop();
    }
}

void Motor_Stop(void) {
    Motor_Output = 0;
    PWM_SetCompare2(0);
    GPIO_ResetBits(GPIOA, GPIO_Pin_2 | GPIO_Pin_3);
}

int16_t Motor_GetOutput(void) {
    return Motor_Output;
}
