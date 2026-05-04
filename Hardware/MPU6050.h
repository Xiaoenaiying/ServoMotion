#ifndef MPU6050_H
#define MPU6050_H
#include "stm32f10x.h"
#include "Common_IMU.h"

typedef struct{
    float ac_x;\
    float ac_y;\
    float ac_z;
} ACommon ;
typedef struct {
    float gy_x,gy_y,gy_z;
}GyroCommon;
void MPU_6050_WriteReg(uint8_t RegAddress, uint8_t Data);

uint8_t MPU_6050_ReadReg(uint8_t RegAddress);

void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);

void MPU_6050_Init(void);

uint8_t MPU_6050_GetID(void);

void MPU_6050_UpdataValue(void);

void MPU6050_FillGyroAccel(GyroAccel_Struct *data);
float MPU_6050_GetaccX(void);
float MPU_6050_GetaccY(void);
float MPU_6050_GetgyroX(void);
float MPU_6050_GetgyroY(void);
float MPU_6050_GetgyroZ(void);
float MPU_6050_Gettemp(void);
float MPU_6050_GetaccZ(void);

#endif
