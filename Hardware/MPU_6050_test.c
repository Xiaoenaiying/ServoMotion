#include "stm32f10x.h"                  // Device header
#include "usart.h"
#include "string.h"
#include "MPU6050.h"
#include "task.h"
#include "delay.h"
#include "Common_IMU.h"

GyroAccel_Struct Data;
float AX, AY, AZ,GX,GY,GZ,Tempture;
EulerAngle_Struct euler = {0};
float M=0.01f;
void APP_USART2Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode= USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);

}

void Usart2_pro(void)
{
    PeriOdic(30);
    My_USART_Printf(USART2,"%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\r\n",
    AX,AY,AZ,GX,GY,GZ,Tempture,euler.roll, euler.pitch, euler.yaw);
}

void MPU_6050_eularTest(void) {

    Delay_Init();
    MPU_6050_Init();
    APP_USART2Init();
    SysTick_Handler();
    while(1) {
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
        Usart2_pro();
    }

}


int main(void) {

    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_0 );
    MPU_6050_eularTest();

}