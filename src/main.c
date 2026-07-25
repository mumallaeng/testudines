#include "stm32f4xx.h"
#include "servo.h"
#include "servo_pwm.h"
#include "uart2.h"

#define USER_LED_PIN 5U

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
    Uart2_Init();
    Servo_ApplyInitialPose();

    Uart2_SendString("Servo key console\r\n");
    Uart2_SendString("q/a BS  w/s SH  e/d EL\r\n");
    Uart2_SendString("r/f WP  t/g WY  y/h GR\r\n");

    while (1)
    {
        uint8_t key = Uart2_ReceiveByte();
        Servo_HandleCalibrationKey(key);
        GPIOA->ODR ^= (1U << USER_LED_PIN);
    }
}
