//
// Created by LENOVO on 2026/4/20.
//

#ifndef SERVOMOTION_PID_H
#define SERVOMOTION_PID_H

#include "stm32f10x.h"
#include "delay.h"
typedef struct {\
    float kP;//比例系数
    float kI;//积分系数
    float kD;//微分系数
    float sP;//设定值
    uint64_t t_k_1;//上一次运行时间
    float err_k_1;//上一次运行的误差值
    float err_int_1;//上一次运行的积分项
}PID_struct;
void PID_Init(PID_struct *PID,float kP, float kI, float kD);
void PID_Set_SP(PID_struct *PID, float sP);
float PID_Computer(PID_struct *PID,float FB);

#endif //SERVOMOTION_PID_H
