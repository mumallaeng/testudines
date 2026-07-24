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

/*
 * Calibration table: Update only this table after testing one powered joint at
 * a time. A zero safe limit is unverified; an initial value of 0U disables that
 * servo's physical PWM output.
 */
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

    /* safety: Apply a limit only after both mechanical limits have been measured. */
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

int Servo_SetAngle(ArmServoId servo, uint16_t angle_deg)
{
    const ServoCalibration *calibration;
    uint16_t effective_angle_deg;
    uint32_t pulse_range_us;
    uint32_t pulse_us;

    /* input: A logical angle is valid only after this joint's mechanical limits are known. */
    if ((servo >= ARM_SERVO_COUNT) || (angle_deg > SERVO_ANGLE_MAX_DEG))
    {
        return 0;
    }

    calibration = &g_servo_calibration[servo];
    if ((calibration->safe_min_us == 0U) ||
        (calibration->safe_max_us == 0U) ||
        (calibration->safe_min_us >= calibration->safe_max_us))
    {
        return 0;
    }

    /* direction: A negative joint reverses the logical angle without changing its measured limits. */
    effective_angle_deg = angle_deg;
    if (calibration->direction == SERVO_DIRECTION_NEGATIVE)
    {
        effective_angle_deg = SERVO_ANGLE_MAX_DEG - angle_deg;
    }

    /* map: pulse = min + round(angle * (max - min) / 180). */
    pulse_range_us = calibration->safe_max_us - calibration->safe_min_us;
    pulse_us = calibration->safe_min_us +
               ((effective_angle_deg * pulse_range_us + (SERVO_ANGLE_MAX_DEG / 2U)) /
                SERVO_ANGLE_MAX_DEG);

    ServoPwm_SetPulseUs(servo, (uint16_t)pulse_us);
    return 1;
}

void Servo_ApplyInitialPose(void)
{
    ArmServoId servo;

    /* pose: An initial value of 0U keeps an untested servo electrically inactive. */
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
