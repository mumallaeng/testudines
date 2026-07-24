#include "stm32f4xx.h"
#include "servo.h"
#include "servo_pwm.h"

#define USER_LED_PIN 5U

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

void testudines(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER &= ~GPIO_MODER_MODER5_Msk;
    GPIOA->MODER |= GPIO_MODER_MODER5_0;

    ServoPwm_InitAll();
    Servo_ApplyInitialPose();

    while (1)
    {
        GPIOA->ODR ^= (1U << USER_LED_PIN);
        Delay(800000U);
    }
}
