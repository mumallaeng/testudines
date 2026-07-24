#ifndef SERVO_PWM_H
#define SERVO_PWM_H

#include <stdint.h>

typedef enum
{
    ARM_SERVO_LOWER_1 = 0,
    ARM_SERVO_LOWER_2,
    ARM_SERVO_LOWER_3,
    ARM_SERVO_UPPER_1,
    ARM_SERVO_UPPER_2,
    ARM_SERVO_GRIPPER,
    ARM_SERVO_COUNT
} ArmServoId;

void ServoPwm_InitAll(void);
void ServoPwm_SetPulseUs(ArmServoId servo, uint16_t pulse_us);
void ServoPwm_DisableOutput(ArmServoId servo);

#endif /* SERVO_PWM_H */
