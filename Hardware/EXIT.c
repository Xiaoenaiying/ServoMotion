//
// Created by LENOVO on 2026/5/10.
//

#include "EXIT.h"
#include "MPU6050.h"
#include "usart.h"
#include "Servo.h"

GyroAccel_Struct Data;
static float AX, AY, AZ,GX,GY,GZ,Tempture;
static EulerAngle_Struct euler = {0};
static float M=0.01f;

/*
 * @简介：EXIT中断I2C处理函数
*/
void EXIT_I2C(void) {
    MPU6050_FillGyroAccel(&Data);
    GyroAccel_Struct fullData;
    fullData.accel.accelX = Data.accel.accelX;
    fullData.accel.accelY = Data.accel.accelY;
    fullData.accel.accelZ = Data.accel.accelZ;
    fullData.gyro.gyroX   = Data.gyro.gyroX;
    fullData.gyro.gyroY   = Data.gyro.gyroY;
    fullData.gyro.gyroZ   = Data.gyro.gyroZ;
    Common_IMU_GetEulerAngle(&fullData,&euler,M);
    AX=MPU_6050_GetaccX();
    AY=MPU_6050_GetaccY();
    AZ=Common_IMU_GetNormAccZ();
    GX=MPU_6050_GetgyroX();
    GY=MPU_6050_GetgyroY();
    GZ=MPU_6050_GetgyroZ();
    Tempture=MPU_6050_Gettemp();

    NVIC_InitTypeDef NVIC_InitI2CStructure;
    NVIC_InitI2CStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
    NVIC_InitI2CStructure.NVIC_IRQChannelPreemptionPriority=2;
    NVIC_InitI2CStructure.NVIC_IRQChannelSubPriority=0;
    NVIC_InitI2CStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitI2CStructure);
}

/*
 * @简介：EXIT中断usart处理函数
*/
void EXIT_USART(void) {
    My_USART_Printf(USART1,"%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\r\n",AX,AY,AZ,GX,GY,GZ,Tempture,euler.roll, euler.pitch, euler.yaw);
    NVIC_InitTypeDef NVIC_InitUSARTStructure;
    NVIC_InitUSARTStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitUSARTStructure.NVIC_IRQChannelPreemptionPriority=3;
    NVIC_InitUSARTStructure.NVIC_IRQChannelSubPriority=0;
    NVIC_InitUSARTStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitUSARTStructure);
}

/*
 * @简介：EXIT中断舵机处理函数
*/
void EXIT_Servo(void) {
    Servo_SetAngle3(euler.pitch);
    NVIC_InitTypeDef NVIC_InitPWMStructure;
    NVIC_InitPWMStructure.NVIC_IRQChannel =TIM2_IRQn;
    NVIC_InitPWMStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitPWMStructure.NVIC_IRQChannelSubPriority=1;
    NVIC_InitPWMStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitPWMStructure);
}