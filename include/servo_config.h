#ifndef SERVO_CONFIG_H
#define SERVO_CONFIG_H

#define SERVO_PWM_HZ                 50U       /* Servo command period: 20 ms */
#define SERVO_NEUTRAL_US             1500U     /* First calibration pulse: 1.5 ms */

/*
 * Per-joint minimum and maximum pulse values stay undefined until mechanical
 * limits and servo directions are calibrated with external servo power.
 */

#endif /* SERVO_CONFIG_H */
