#include "stm32f10x.h"                  // Device header
#include "usart.h"
#include "string.h"
#include "MPU6050.h"
#include "task.h"
#include "delay.h"
#include "Common_IMU.h"
#include "Servo.h"
#include "NVIC.h"

GyroAccel_Struct Data;
static float AX, AY, AZ,GX,GY,GZ,Tempture;
static EulerAngle_Struct euler = {0};
static float M=0.01f;


void Usart_pro(void)
{
    PeriOdic(30);
    My_USART_Printf(USART1,"%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\r\n",AX,AY,AZ,GX,GY,GZ,Tempture,euler.roll, euler.pitch, euler.yaw);
    Servo_SetAngle3(euler.pitch);
}

void MPU_6050_eularTest(void) {

    Delay_Init();
    MPU_6050_Init();
    APP_USARTInit(GPIO_Pin_9,GPIO_Pin_10,115200);
    Servo_Init();
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
        Usart_pro();
    }

}


int main(void) {

    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 );
    NVIC_InitOther();
    MPU_6050_eularTest();

}