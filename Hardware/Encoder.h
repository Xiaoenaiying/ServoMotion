#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f10x.h"

#define ENCODER_PPR 11.0f
#define ENCODER_MULTIPLY 4.0f
#define ENCODER_GEAR_RATIO 30.0f
#define ENCODER_COUNTS_PER_OUTPUT_REV (ENCODER_PPR * ENCODER_MULTIPLY * ENCODER_GEAR_RATIO)

void Encoder_Init(void);

void Encoder_Update(void);

int32_t Encoder_GetCount(void);

int16_t Encoder_GetDelta(void);

float Encoder_GetAngle(void);

float Encoder_GetSpeedRPM(float dt);

int32_t Encoder_GetRawCounter(void);

uint32_t Encoder_GetIrqCount(void); // EXTI中断触发次数
uint32_t Encoder_GetValidCount(void); // 合法正交跳变次数
uint32_t Encoder_GetInvalidCount(void); // 非法跳变次数
uint8_t Encoder_GetLastChange(void); // 最近一次跳变：上次AB << 2 | 当前AB
uint32_t Encoder_GetChangeCount(uint8_t Change); // 查询某一种跳变出现次数，Change范围0x00~0x0F
uint8_t Encoder_GetPhaseA(void);

uint8_t Encoder_GetPhaseB(void);

void Encoder_Reset(void);

#endif
