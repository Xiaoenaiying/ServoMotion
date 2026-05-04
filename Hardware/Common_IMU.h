//
// Created by LENOVO on 2026/4/19.
//

#ifndef SERVOMOTION_COMMON_IMU_H
#define SERVOMOTION_COMMON_IMU_H


typedef struct {
     struct {
          float accelX;   // 加速度计 X 轴（单位：g 或 m/s²）
          float accelY;   // 加速度计 Y 轴
          float accelZ;   // 加速度计 Z 轴
     } accel;
     struct {
          float gyroX;    // 陀螺仪 X 轴（单位：度/秒）
          float gyroY;    // 陀螺仪 Y 轴
          float gyroZ;    // 陀螺仪 Z 轴
     } gyro;
} GyroAccel_Struct;




typedef struct {
     float roll;   // 横滚角，单位：度
     float pitch;  // 俯仰角，单位：度
     float yaw;    // 偏航角，单位：度
} EulerAngle_Struct;



#include "math.h"

/* 表示四元数的结构体 */
typedef struct\
{\
     float q0;\
     float q1;\
     float q2;\
     float q3;\
} Quaternion_Struct;
extern float RtA;
extern float Gyro_G;
extern float Gyro_Gr;

void Common_IMU_GetEulerAngle(GyroAccel_Struct *gyroAccel,EulerAngle_Struct *eulerAngle,float dt);
float Common_IMU_GetNormAccZ(void);
#endif //SERVOMOTION_COMMON_IMU_H
