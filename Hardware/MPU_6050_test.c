#include "stm32f10x.h"                  // Device header
#include "usart.h"
#include "string.h"
#include "MPU6050.h"
#include "task.h"
#include "delay.h"
#include "Common_IMU.h"
#include "Servo.h"
#include "EXIT.h"

EulerAngle_Struct euler = {0};
float M=0.01f;

void Usart_pro(void)
{
    PeriOdic(30);
    EXIT_USART();
    EXIT_Servo();
}

void MPU_6050_eularTest(void) {

    Delay_Init();
    MPU_6050_Init();
    APP_USARTInit(GPIO_Pin_9,GPIO_Pin_10,115200);
    Servo_Init();
    SysTick_Handler();
    while(1) {
        EXIT_I2C();
        Usart_pro();
    }

}


int main(void) {

    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 );
    MPU_6050_eularTest();

}