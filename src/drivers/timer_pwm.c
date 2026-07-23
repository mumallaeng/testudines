#include "stm32f4xx.h"
#include "servo_config.h"

#define TIMER_INPUT_HZ 16000000U // reset HSI: 16 MHz
#define TIMER_TICK_HZ 1000000U   // timer tick: 1 µs

#define TIM4_PSC_VALUE ((TIMER_INPUT_HZ / TIMER_TICK_HZ) - 1U) // 16 MHz / 16 = 1 MHz
#define TIM4_ARR_VALUE ((TIMER_TICK_HZ / SERVO_PWM_HZ) - 1U)   // 1 MHz / 50 - 1 = 19,999

void ServoPwm_Lower1_Init(void)
{
    /* clock: Enable clocks for GPIOB and TIM4 peripherals. */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    /* pin mux: Route TIM4_CH1 to PB6 by selecting alternate-function mode and AF2. */
    GPIOB->MODER &= ~GPIO_MODER_MODER6_Msk;
    GPIOB->MODER |= GPIO_MODER_MODER6_1;

    GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL6_Msk;
    GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_1;

    /* time base: Configure the TIM4 time base: 1 µs tick and 20 ms PWM period. */
    TIM4->CR1 = 0U;
    TIM4->PSC = TIM4_PSC_VALUE;
    TIM4->ARR = TIM4_ARR_VALUE;
    TIM4->CCR1 = SERVO_NEUTRAL_US;

    /* PWM channel: Configure TIM4 channel 1 for PWM mode 1 and enable its output. */
    TIM4->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_OC1M);
    TIM4->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE;
    TIM4->CCER |= TIM_CCER_CC1E;

    /* start: Latch preload registers, then start the counter. */
    TIM4->EGR = TIM_EGR_UG;
    TIM4->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;
}