#include "stm32f10x.h"
#include "MPU6050_Reg.h"
#include "math.h"
#include "task.h"
#include "MPU6050.h"
#include "Common_Filter.h"

//
//ax_raw,ay_raw,az_raw,gx_raw,gy_raw,gz_raw存储原始ax,ay,az,gx,gy,gz，温度
//
float ax_raw,ay_raw,az_raw;
float gx_raw,gy_raw,gz_raw;
float temperature_raw;
/*
 *卡尔曼滤波处理加速度后的数据
 */
double ax_rawCrman,ay_rawCrman,az_rawCrman;
/*
 *低通滤波后的值：ax,ay,az,gx,gy,gz，温度
 */
float gx_Lraw,gy_Lraw,gz_Lraw;
float ax_Lraw,ay_Lraw,az_Lraw;
float temperature_Lraw;
//MPU6050的地址
#define MPU_6050_Address 0xD0
/**
  * 函    数：MPU6050等待事件
  * 参    数：同I2C_CheckEvent
  * 返 回 值：无
  */
void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
    uint32_t Timeout;
    Timeout = 10000;									//给定超时计数时间
    while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)	//循环等待指定事件
    {
        Timeout --;										//等待时，计数值自减
        if (Timeout == 0)								//自减到0后，等待超时
        {
            /*超时的错误处理代码，可以添加到此处*/
            break;										//跳出等待，不等了
        }
    }
}
/*
 *@简介：向MPU6050写入数据
 */
void MPU_6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{

    I2C_GenerateSTART(I2C2, ENABLE);
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)!=SUCCESS);

    I2C_Send7bitAddress(I2C2, MPU_6050_Address, I2C_Direction_Transmitter);
    while (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)!=SUCCESS);

    I2C_SendData(I2C2, RegAddress);
    while (I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTING)!=SUCCESS);

    I2C_SendData(I2C2, Data);
    while (I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED)!=SUCCESS);

    I2C_GenerateSTOP(I2C2, ENABLE);
}
//
//读取MPU6050寄存器的值
//
uint8_t MPU_6050_ReadReg(uint8_t RegAddress)
{
    uint8_t Data;

    I2C_GenerateSTART(I2C2, ENABLE);
    MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);

    I2C_Send7bitAddress(I2C2, MPU_6050_Address, I2C_Direction_Transmitter);
    MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

    I2C_SendData(I2C2, RegAddress);
    MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    I2C_GenerateSTART(I2C2, ENABLE);
    MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);

    I2C_Send7bitAddress(I2C2, MPU_6050_Address, I2C_Direction_Receiver);
    MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);

    I2C_AcknowledgeConfig(I2C2, DISABLE);
    I2C_GenerateSTOP(I2C2, ENABLE);

    MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED);
    Data=I2C_ReceiveData(I2C2);

    I2C_AcknowledgeConfig(I2C2, ENABLE);

    return Data;
}

//
//读取任意长度的数据
//
uint8_t MPU_6050_ReadOtherlong(uint8_t ReadAddress, uint8_t *pData, uint8_t Length) {
    I2C_GenerateSTART(I2C2, ENABLE);
    MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);

    I2C_Send7bitAddress(I2C2, MPU_6050_Address, I2C_Direction_Transmitter);
    MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

    I2C_SendData(I2C2, ReadAddress);
    MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTING);

    I2C_GenerateSTART(I2C2, ENABLE);
    MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);

    I2C_Send7bitAddress(I2C2, MPU_6050_Address, I2C_Direction_Receiver);
    MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);

    while (Length)
    {
        if (Length == 1)
        {
            I2C_AcknowledgeConfig(I2C2, DISABLE);
            I2C_GenerateSTOP(I2C2, ENABLE);
        }
        MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED);
        *pData = I2C_ReceiveData(I2C2);
        pData++;
        Length--;
    }
    I2C_AcknowledgeConfig(I2C2, ENABLE);
    return SUCCESS;
}

//
//MPU6050初始化
//
void MPU_6050_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    GPIO_InitTypeDef GPIO_I2C_InitStruct;
    GPIO_I2C_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_I2C_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_I2C_InitStruct.GPIO_Pin=GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_I2C_InitStruct);//初始化I2C引脚PB10,PB11

    //开启RCC I2C时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);

    //I2C初始化配置
    I2C_InitTypeDef I2C_InitStructure;
    I2C_InitStructure.I2C_ClockSpeed=400000;
    I2C_InitStructure.I2C_DutyCycle=I2C_DutyCycle_2;
    I2C_InitStructure.I2C_Mode=I2C_Mode_I2C;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_Init(I2C2, &I2C_InitStructure);
    I2C_Cmd(I2C2, ENABLE);

    MPU_6050_WriteReg(MPU6050_PWR_MGMT_1,0x01);
    MPU_6050_WriteReg(MPU6050_PWR_MGMT_2,0x00);
    MPU_6050_WriteReg(MPU6050_SMPLRT_DIV,0x09);
    MPU_6050_WriteReg(MPU6050_CONFIG,0x06);
    MPU_6050_WriteReg(MPU6050_GYRO_CONFIG,0x18);
    MPU_6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);
}
uint8_t MPU_6050_GetID(void) {
    return MPU_6050_ReadReg(MPU6050_WHO_AM_I);
}
/**
 * @brief 更新MPU6050值
 * @param accx,accy,,accz,gyrox,gyroy,gyroz,celsius存储原始值计算结果
 */
void MPU_6050_UpdataValue(void)
{
    uint8_t data[14];
    static int16_t accX,accY,accZ,GyroX,GyroY,GyroZ;
    static int16_t Celsius;

    //
    //进行移位计算把高八位和低八位合并
    //
    MPU_6050_ReadOtherlong(MPU6050_ACCEL_XOUT_H,data,14);
    accX=(int16_t)(data[0]<<8|data[1]);
    accY=(int16_t)(data[2]<<8|data[3]);
    accZ=(int16_t)(data[4]<<8|data[5]);
    Celsius=(int16_t)(data[6]<<8|data[7]);
    GyroX=(int16_t)(data[8]<<8|data[9]);
    GyroY=(int16_t)(data[10]<<8|data[11]);
    GyroZ=(int16_t)(data[12]<<8|data[13]);
    ax_raw=accX*4.8828125e-4f;
    ay_raw=accY*4.8828125e-4f;
    az_raw=accZ*4.8828125e-4f;
    temperature_raw=Celsius/340+36.53f;
    gx_raw=GyroX*6.1035e-2f;
    gy_raw=GyroY*6.1035e-2f;
    gz_raw=GyroZ*6.1035e-2f;
;
    //
    //@简介：对原始数据先进行低通滤波处理
    //
    ACommon low_precision_acceleration_data;
    low_precision_acceleration_data.ac_x=Common_Filter_LowPass(ax_raw,ax_Lraw);
    low_precision_acceleration_data.ac_y=Common_Filter_LowPass(ay_raw,ay_Lraw);
    low_precision_acceleration_data.ac_z=Common_Filter_LowPass(az_raw,az_Lraw);
    ax_Lraw=low_precision_acceleration_data.ac_x;
    ay_Lraw=low_precision_acceleration_data.ac_y;
    az_Lraw=low_precision_acceleration_data.ac_z;

    GyroCommon low_precision_gyro_data;
    low_precision_gyro_data.gy_x=Common_Filter_LowPass(gx_raw,gx_Lraw);
    low_precision_gyro_data.gy_y=Common_Filter_LowPass(gy_raw,gy_Lraw);
    low_precision_gyro_data.gy_z=Common_Filter_LowPass(gz_raw,gz_Lraw);
    gx_Lraw=low_precision_gyro_data.gy_x;
    gy_Lraw=low_precision_gyro_data.gy_y;
    gz_Lraw=low_precision_gyro_data.gy_z;

    temperature_Lraw=Common_Filter_LowPass(temperature_raw,temperature_Lraw);

    //
    //@简介：加速度低通滤波值进行卡尔曼滤波处理
    //
    ax_rawCrman=Common_Filter_KalmanFilter(&kfs[0],low_precision_acceleration_data.ac_x);
    ay_rawCrman=Common_Filter_KalmanFilter(&kfs[1],low_precision_acceleration_data.ac_y);
    az_rawCrman=Common_Filter_KalmanFilter(&kfs[2],low_precision_acceleration_data.ac_z);
}


/**
 * @brief 将MPU6050原始数据填入GyroAccel_Struct
 * @param data 指向结构体的指针
 */
void MPU6050_FillGyroAccel(GyroAccel_Struct *data)
{
    PeriOdic(10);
    MPU_6050_UpdataValue();   // 刷新原始数据到 ax_raw, gx_raw 等

    // 将数据填入对应结构体
    data->accel.accelX = ax_rawCrman;
    data->accel.accelY = ay_rawCrman;
    data->accel.accelZ = az_rawCrman;

    data->gyro.gyroX = gx_Lraw;
    data->gyro.gyroY = gy_Lraw;
    data->gyro.gyroZ = gz_Lraw;
}
//
//读取卡尔曼滤波后的X轴加速度
//
float MPU_6050_GetaccX(void) {
    return ax_rawCrman;
}
//
//读取卡尔曼滤波后的y轴加速度
//
float MPU_6050_GetaccY(void) {
    return ay_rawCrman;
}
//
//读取卡尔曼滤波后的z轴加速度
//
float MPU_6050_GetaccZ(void) {
    return az_rawCrman;
}
//
//读取低通滤波滤波后的X轴角速度
//
float MPU_6050_GetgyroX(void) {
    return gx_Lraw;
}
//
//读取低通滤波滤波后的y轴角速度
//
float MPU_6050_GetgyroY(void) {
    return gy_Lraw;
}
//
//读取低通滤波滤波后的z轴角速度
//
float MPU_6050_GetgyroZ(void) {
    return gz_Lraw;
}
//
//读取低通滤波滤波后的温度
//
float MPU_6050_Gettemp(void) {
    return temperature_Lraw;
}