//
// Created by LENOVO on 2026/5/10.
//

#include "NVIC.h"
#include "stm32f10x.h"

/*
 * @简介：NVIC配置处理函数
*/
void NVIC_InitOther(void) {

    //配置I2C的NVIC
    NVIC_InitTypeDef NVIC_InitI2CStructure;
    NVIC_InitI2CStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
    NVIC_InitI2CStructure.NVIC_IRQChannelPreemptionPriority=2;
    NVIC_InitI2CStructure.NVIC_IRQChannelSubPriority=0;
    NVIC_InitI2CStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitI2CStructure);

    //配置USART的NVIC
    NVIC_InitTypeDef NVIC_InitUSARTStructure;
    NVIC_InitUSARTStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitUSARTStructure.NVIC_IRQChannelPreemptionPriority=3;
    NVIC_InitUSARTStructure.NVIC_IRQChannelSubPriority=0;
    NVIC_InitUSARTStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitUSARTStructure);

    //配置TIM2的NVIC
    NVIC_InitTypeDef NVIC_InitPWMStructure;
    NVIC_InitPWMStructure.NVIC_IRQChannel =TIM2_IRQn;
    NVIC_InitPWMStructure.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitPWMStructure.NVIC_IRQChannelSubPriority=1;
    NVIC_InitPWMStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitPWMStructure);
    }