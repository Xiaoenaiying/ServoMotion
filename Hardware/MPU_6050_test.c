#include "stm32f10x.h"                  // Device header
#include "usart.h"
#include "task.h"
#include "delay.h"
#include "Motor.h"
#include "Encoder.h"
#include "NVIC.h"

static int32_t EncoderCount = 0;
static float EncoderAngle = 0.0f;
static int16_t EncoderDelta = 0;
static int32_t EncoderRpmX100 = 0;
static int16_t MotorOutput = 0;
static int32_t EncoderRaw = 0;
static uint32_t EncoderIrq = 0;
static uint32_t EncoderValid = 0;
static uint32_t EncoderInvalid = 0;
static uint8_t EncoderLastChange = 0;
static uint32_t EncoderSameChange = 0;
static uint32_t EncoderJumpChange = 0;
static uint8_t EncoderA = 0;
static uint8_t EncoderB = 0;


void Usart_pro(void) {
    PeriOdic(100);

    /*
     * 调试输出说明：
     * count/rpm/angle 是正常编码器结果。
     * irq/valid/invalid/last/same/jump 是排查编码器信号质量用的诊断量。
     *   irq     : PA6/PA7边沿中断次数
     *   valid   : 合法正交跳变次数，正常手转/电机转时应该增加
     *   invalid : 非法跳变次数，毛刺、干扰、重复中断时会增加
     *   last    : 最近一次跳变，格式是 上次AB << 2 | 当前AB
     *   same    : 状态没变类跳变总数，例如 11->11，常见于窄毛刺/重复中断
     *   jump    : A/B同时跳变总数，例如 11->00，常见于干扰/漏采/短接
     *   A/B     : 当前PA6/PA7电平，只代表打印瞬间的电平
     */
    My_USART_Printf(
        USART1,
        "count=%ld,delta=%d,rpm_x100=%ld,angle_x100=%ld,out=%d,raw=%ld,irq=%lu,valid=%lu,invalid=%lu,last=%02X,same=%lu,jump=%lu,A=%d,B=%d\r\n",
        EncoderCount, EncoderDelta, EncoderRpmX100, (int32_t) (EncoderAngle * 100.0f),
        MotorOutput, EncoderRaw, EncoderIrq, EncoderValid, EncoderInvalid, EncoderLastChange,
        EncoderSameChange, EncoderJumpChange, EncoderA, EncoderB);
}

void Motor_pro(void) {
    PeriOdic(10);

    EncoderDelta = Encoder_GetDelta();
    EncoderRpmX100 = (int32_t) ((float) EncoderDelta * 600000.0f / ENCODER_COUNTS_PER_OUTPUT_REV);
    EncoderCount = Encoder_GetCount();
    EncoderAngle = Encoder_GetAngle();
    EncoderRaw = Encoder_GetRawCounter();

    /* 编码器诊断数据，用来判断是正常A/B方波还是毛刺/干扰 */
    EncoderIrq = Encoder_GetIrqCount();
    EncoderValid = Encoder_GetValidCount();
    EncoderInvalid = Encoder_GetInvalidCount();
    EncoderLastChange = Encoder_GetLastChange();

    /*
     * same统计状态没变的4种情况：
     *   0x00 = 00 -> 00
     *   0x05 = 01 -> 01
     *   0x0A = 10 -> 10
     *   0x0F = 11 -> 11
     * same很大时，通常说明有窄毛刺触发中断，但进中断读GPIO时电平已经回去了。
     */
    EncoderSameChange = Encoder_GetChangeCount(0x00) + Encoder_GetChangeCount(0x05) +
                        Encoder_GetChangeCount(0x0A) + Encoder_GetChangeCount(0x0F);

    /*
     * jump统计A/B同时变化的4种情况：
     *   0x03 = 00 -> 11
     *   0x06 = 01 -> 10
     *   0x09 = 10 -> 01
     *   0x0C = 11 -> 00
     * jump很大时，通常说明A/B受干扰、线短接、或者软件漏采了中间状态。
     */
    EncoderJumpChange = Encoder_GetChangeCount(0x03) + Encoder_GetChangeCount(0x06) +
                        Encoder_GetChangeCount(0x09) + Encoder_GetChangeCount(0x0C);

    EncoderA = Encoder_GetPhaseA();
    EncoderB = Encoder_GetPhaseB();

    MotorOutput = 200;
    Motor_SetOutput(MotorOutput);
    // Motor_SetOutput(MotorOutput);
}

void MotorEncoder_Test(void) {
    Delay_Init();
    APP_USARTInit(GPIO_Pin_9,GPIO_Pin_10, 115200);
    Motor_Init();
    Encoder_Init();
    while (1) {
        Motor_pro();
        Usart_pro();
    }
}


int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitOther();
    MotorEncoder_Test();
}
