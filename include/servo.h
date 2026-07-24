#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

#include "servo_pwm.h"

#define SERVO_ANGLE_MIN_DEG 0U
#define SERVO_ANGLE_MAX_DEG 180U

void Servo_ApplyInitialPose(void);

#endif /* SERVO_H */
