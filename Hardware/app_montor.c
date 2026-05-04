//
// Created by LENOVO on 2026/4/24.
//

#include "app_montor.h"
#include "stm32f10x.h"
#include "Pid.h"
#include "task.h"
#include "Common_IMU.h"
#include "PWM.h"
static PID_struct PID_Servo_First;
static PID_struct PID_Servo_Second;
static PID_struct PID_Servo_Thirte;
/**
* @brief 初始化舵机PID
* @param PID_Servo_First到PID_Servo_Thirte三台舵机系数
*/
void App_Montor_Init(void) {
    PID_Init(&PID_Servo_First,0.5,7,0);
    PID_Init(&PID_Servo_Second,0.5,7,0);
    PID_Init(&PID_Servo_Thirte,0.5,7,0);
}
void APP_Montor_Proc(void) {
    PeriOdic(1)//每间隔一ms执行一次
    static uint8_t m=10;
    PID_Set_SP(&PID_Servo_First,m);
    GyroAccel_Struct Servo_First;
    EulerAngle_Struct Euler_First;
    Common_IMU_GetEulerAngle(&Servo_First,&Euler_First,10);
    volatile typedef struct  {
        float Servo_First_Pitch;
        float Servo_First_Yaw;
        float Servo_First_Roll;
    }ServoStruct_First_EulerAngle;
    ServoStruct_First_EulerAngle Servo_First_EulerAngle;
    Servo_First_EulerAngle.Servo_First_Pitch =PID_Computer(&PID_Servo_First,Euler_First.pitch);
    Servo_First_EulerAngle.Servo_First_Roll=PID_Computer(&PID_Servo_First,Euler_First.roll);
    Servo_First_EulerAngle.Servo_First_Yaw=PID_Computer(&PID_Servo_First,Euler_First.yaw);

}