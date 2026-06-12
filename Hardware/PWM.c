#include "stm32f10x.h"                  // Device header
#include "PWM.h"

/**
  * 函    数：PWM初始化
  * 参    数：无
  * 返 回 值：无
  */
void PWM_Init(void) {
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //开启TIM2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //开启GPIOA的时钟

	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); //PA1作为TIM2_CH2电机PWM输出

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure); //PA2/PA3作为电机方向控制IO
	GPIO_ResetBits(GPIOA, GPIO_Pin_2 | GPIO_Pin_3);

	/*配置时钟源*/
	TIM_InternalClockConfig(TIM2); //选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟

	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; //定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; //时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = PWM_MOTOR_PERIOD; //20kHz电机PWM周期
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0; //72MHz / (3599 + 1) = 20kHz
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0; //重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure); //将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元

	/*输出比较初始化*/
	TIM_OCInitTypeDef TIM_OCInitStructure; //定义结构体变量
	TIM_OCStructInit(&TIM_OCInitStructure); //结构体初始化，若结构体没有完整赋值
	//则最好执行此函数，给结构体所有成员都赋一个默认值
	//避免结构体初值不确定的问题
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //输出比较模式，选择PWM模式1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性，选择为高，若选择极性为低，则输出高低电平取反
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //输出使能
	TIM_OCInitStructure.TIM_Pulse = 0; //初始的CCR值
	TIM_OC2Init(TIM2, &TIM_OCInitStructure); //PA1/TIM2_CH2输出电机PWM

	/*TIM使能*/
	TIM_Cmd(TIM2, ENABLE); //使能TIM2，定时器开始运行
}

/**
  * 函    数：PWM设置CCR
  * 参    数：Compare 要写入的CCR的值，范围：0~PWM_MOTOR_PERIOD
  * 返 回 值：无
  * 注意事项：CCR和ARR共同决定占空比，此函数仅设置CCR的值，并不直接是占空比
  *           占空比Duty = CCR / (ARR + 1)
  */
void PWM_SetCompare2(uint16_t Compare) {
	if (Compare > PWM_MOTOR_PERIOD) Compare = PWM_MOTOR_PERIOD;
	TIM_SetCompare2(TIM2, Compare); //设置CCR2的值
}

/**
  * 函    数：PWM设置CCR (通道3)
  * 参    数：Compare 要写入的CCR的值
  * 返 回 值：无
  */
void PWM_SetCompare3(uint16_t Compare) {
	(void) Compare;
}

/**
  * 函    数：PWM设置CCR (通道4)
  * 参    数：Compare 要写入的CCR的值
  * 返 回 值：无
  */
void PWM_SetCompare4(uint16_t Compare) {
	(void) Compare;
}

uint16_t PWM_GetPeriod(void) {
	return PWM_MOTOR_PERIOD;
}
