#include "stm32f4xx.h"

#define USER_LED_PIN 5U
#define GPIOAEN 0U

static void Delay(volatile uint32_t count)
{
    while (count-- != 0U)
        ;
}

void _Invalid_ISR(void) // startup vector table의 exception(interrupt) 기본 처리
{
    while (1)
        ;
}

void ServoPwm_Lower1_Init(void);

void testudines(void)
{
    RCC->AHB1ENR |= (1U << GPIOAEN);

    GPIOA->MODER &= ~(3U << (USER_LED_PIN * 2U));
    GPIOA->MODER |= (1U << (USER_LED_PIN * 2U));

    ServoPwm_Lower1_Init();

    while (1)
    {
        GPIOA->ODR ^= (1U << USER_LED_PIN);
        Delay(800000U);
    }
}