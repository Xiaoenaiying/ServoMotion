//
// Created by LENOVO on 2026/4/20.
//

#include "Pid.h"
/**
 * @brief 初始化PID
 * @param PID作为结构体，保存数据
 * @param kP比例系数，kI积分系数，KD微分系数
 */
void PID_Init(PID_struct *PID,float kP, float kI, float kD) {
    PID->kP = kP;
    PID->kI = kI;
    PID->kD = kD;
    PID->sP = 0.0f;
    PID->t_k_1=0;
    PID->err_k_1=0.0f;
    PID->err_int_1=0.0f;
}

/**
 * @brief 修改设定值
 * @param SP设定值
 */
void PID_Set_SP(PID_struct *PID, float sP) {
    PID->sP = sP;
}

/**
 * @brief PID输出舵机位置
 * @param FB为传入的传感器的值
 */
float PID_Computer(PID_struct *PID,float FB) {
    float CO=0.0f;
    float err=(PID->sP)-FB;

    uint64_t t_k=GetUs();

    uint64_t delta_t=t_k-(PID->t_k_1);

    float err_int;
    float err_dev=(err-(PID->err_k_1))/delta_t;
    err_int=(PID->err_int_1)+(err+(PID->err_k_1))*(delta_t/2.0f);

    //
    PID->t_k_1=t_k;
    PID->err_k_1=err;
    PID->err_int_1=err_int;
    float COp=PID->kP*err;
    float COi=PID->kI*err_int;
    float COd=PID->kD*err_dev;

    CO=COp+COi+COd;
    return CO;

}
