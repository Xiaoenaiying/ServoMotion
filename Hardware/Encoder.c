#include "Encoder.h"

/*
 * TIM3编码器模式说明：
 * PA6 = TIM3_CH1 = 编码器A相
 * PA7 = TIM3_CH2 = 编码器B相
 *
 * 之前用EXTI中断在PA6/PA7边沿里软件解码，容易被窄毛刺触发。
 * 现在改成TIM3硬件编码器模式，让定时器外设自己识别A/B正交信号并加减计数。
 */
static volatile int32_t Encoder_Count = 0; // 扩展后的32位累计计数
static int32_t Encoder_LastCount = 0; // 上次读取增量时的累计计数
static uint16_t Encoder_LastTimerCount = 0; // 上次读取到的TIM3->CNT，用来计算本次增量

static void Encoder_UpdateCount(void) {
    uint16_t NowTimerCount = TIM_GetCounter(TIM3);

    /*
     * TIM3是16位计数器，范围0~65535。
     * 把两次读取的差值强制转换成int16_t，可以自动处理16位溢出：
     *   65535 -> 0      会得到 +1
     *   0 -> 65535      会得到 -1
     * 只要两次调用之间实际变化不超过32767个计数，就不会算错。
     */
    int16_t Delta = (int16_t) (NowTimerCount - Encoder_LastTimerCount);

    Encoder_Count += Delta;
    Encoder_LastTimerCount = NowTimerCount;
}

void Encoder_Init(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* PA6/PA7配置为上拉输入，接编码器A/B相 */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* TIM3基础计数器配置为16位满量程 */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

    /*
     * TIM_EncoderMode_TI12：同时使用CH1和CH2两个输入，硬件按A/B相位自动加减计数。
     * TIM_ICPolarity_Rising表示不反相输入。
     * 如果实际方向反了，可以交换PA6/PA7，或把下面两个极性改成Falling试验。
     */
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12,
                               TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    /*
     * 给输入捕获加数字滤波，减少窄毛刺影响。
     * 0x00 = 不滤波，0x0F = 最强滤波。
     * 如果高速转动时丢计数，可以适当调小，比如0x06~0x0A。
     */
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0F;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICFilter = 0x0F;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    TIM_SetCounter(TIM3, 0);
    TIM_Cmd(TIM3, ENABLE);

    Encoder_Count = 0;
    Encoder_LastCount = 0;
    Encoder_LastTimerCount = TIM_GetCounter(TIM3);
}

void Encoder_Update(void) {
    Encoder_UpdateCount();
}

int32_t Encoder_GetCount(void) {
    Encoder_UpdateCount();
    return Encoder_Count;
}

int16_t Encoder_GetDelta(void) {
    int32_t NowCount;
    int32_t Delta;

    Encoder_UpdateCount();

    NowCount = Encoder_Count;
    Delta = NowCount - Encoder_LastCount;
    Encoder_LastCount = NowCount;

    if (Delta > 32767) return 32767;
    if (Delta < -32768) return -32768;

    return (int16_t) Delta;
}

float Encoder_GetAngle(void) {
    return (float) Encoder_GetCount() * 360.0f / ENCODER_COUNTS_PER_OUTPUT_REV;
}

float Encoder_GetSpeedRPM(float dt) {
    int16_t Delta;

    if (dt <= 0.0f) return 0.0f;

    Delta = Encoder_GetDelta();
    return (float) Delta * 60.0f / (ENCODER_COUNTS_PER_OUTPUT_REV * dt);
}

int32_t Encoder_GetRawCounter(void) {
    return TIM_GetCounter(TIM3);
}

uint32_t Encoder_GetIrqCount(void) {
    /* TIM3硬件编码器模式不再使用EXTI中断，所以这里固定返回0。 */
    return 0;
}

uint32_t Encoder_GetValidCount(void) {
    /* 硬件编码器模式不再做软件valid/invalid分类。 */
    return 0;
}

uint32_t Encoder_GetInvalidCount(void) {
    /* 硬件编码器模式不再做软件valid/invalid分类。 */
    return 0;
}

uint8_t Encoder_GetLastChange(void) {
    /* 硬件编码器模式不再记录软件跳变编码。 */
    return 0;
}

uint32_t Encoder_GetChangeCount(uint8_t Change) {
    (void) Change;
    /* 硬件编码器模式不再统计16种软件跳变。 */
    return 0;
}

uint8_t Encoder_GetPhaseA(void) {
    return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
}

uint8_t Encoder_GetPhaseB(void) {
    return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
}

void Encoder_Reset(void) {
    __disable_irq();
    TIM_SetCounter(TIM3, 0);
    Encoder_Count = 0;
    Encoder_LastCount = 0;
    Encoder_LastTimerCount = TIM_GetCounter(TIM3);
    __enable_irq();
}
