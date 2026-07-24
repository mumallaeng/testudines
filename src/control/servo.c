#include <stdint.h>

#include "servo.h"
#include "servo_config.h"
#include "servo_pwm.h"

typedef enum
{
    SERVO_DIRECTION_UNVERIFIED = 0,
    SERVO_DIRECTION_POSITIVE = 1,
    SERVO_DIRECTION_NEGATIVE = -1
} ServoDirection;

typedef struct
{
    uint16_t safe_min_us;
    uint16_t neutral_us;
    uint16_t safe_max_us;
    uint16_t initial_us;
    ServoDirection direction;
} ServoCalibration;

/* Calibration table: 0일 땐 PWM 출력 비활성화 */
static const ServoCalibration g_servo_calibration[ARM_SERVO_COUNT] =
    {
        [ARM_SERVO_LOWER_1] = {0U, SERVO_NEUTRAL_US, 0U, SERVO_NEUTRAL_US, SERVO_DIRECTION_UNVERIFIED},
        [ARM_SERVO_LOWER_2] = {0U, SERVO_NEUTRAL_US, 0U, 800U, SERVO_DIRECTION_UNVERIFIED},
        [ARM_SERVO_LOWER_3] = {0U, SERVO_NEUTRAL_US, 0U, 800U, SERVO_DIRECTION_UNVERIFIED},
        // [ARM_SERVO_LOWER_3] = {0U, SERVO_NEUTRAL_US, 0U, 350U, SERVO_DIRECTION_UNVERIFIED},
        [ARM_SERVO_UPPER_1] = {0U, SERVO_NEUTRAL_US, 0U, 1000U, SERVO_DIRECTION_UNVERIFIED},
        // [ARM_SERVO_UPPER_2] = {0U, SERVO_NEUTRAL_US, 0U, 1500U, SERVO_DIRECTION_UNVERIFIED},
        [ARM_SERVO_UPPER_2] = {0U, SERVO_NEUTRAL_US, 0U, 100U, SERVO_DIRECTION_UNVERIFIED},
        [ARM_SERVO_GRIPPER] = {0U, SERVO_NEUTRAL_US, 0U, 100U, SERVO_DIRECTION_UNVERIFIED}};

static uint16_t Servo_ClampToKnownRange(const ServoCalibration *calibration)
{
    uint16_t pulse_us = calibration->initial_us;

    if ((calibration->safe_min_us != 0U) &&
        (calibration->safe_max_us != 0U) &&
        (calibration->safe_min_us <= calibration->safe_max_us))
    {
        if (pulse_us < calibration->safe_min_us)
        {
            pulse_us = calibration->safe_min_us;
        }
        else if (pulse_us > calibration->safe_max_us)
        {
            pulse_us = calibration->safe_max_us;
        }
    }

    return pulse_us;
}

void Servo_ApplyInitialPose(void)
{
    ArmServoId servo;

    for (servo = ARM_SERVO_LOWER_1; servo < ARM_SERVO_COUNT; servo++)
    {
        if (g_servo_calibration[servo].initial_us == 0U)
        {
            ServoPwm_DisableOutput(servo);
        }
        else
        {
            ServoPwm_SetPulseUs(servo, Servo_ClampToKnownRange(&g_servo_calibration[servo]));
        }
    }
}
