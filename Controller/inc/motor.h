/**
 ******************************************************************************
 * @file    motor.h
 * @brief   Dual DC Motor Control using TIM3 PWM
 ******************************************************************************
 *
 * Hardware Setup:
 *   - Left Motor PWM   -> PA6 (TIM3_CH1)
 *   - Left Motor IN1   -> PA4 (GPIO - direction)
 *   - Left Motor IN2   -> PA5 (GPIO - direction)
 *   - Right Motor PWM  -> PA7 (TIM3_CH2)
 *   - Right Motor IN1  -> PB0 (GPIO - direction)
 *   - Right Motor IN2  -> PB1 (GPIO - direction)
 *
 * PWM Configuration:
 *   - Timer: TIM3
 *   - Frequency: 1kHz
 *   - Resolution: 0-1000 (0.1% steps)
 *
 ******************************************************************************
 */

#ifndef MOTOR_H
#define MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

/* Motor speed range */
#define MOTOR_MAX_SPEED 1000
#define MOTOR_MIN_SPEED (-1000)

/**
 * @brief  Initialize dual motor control (TIM3 PWM on PA6/PA7)
 */
void Motor_Init(void);

/**
 * @brief  Set left motor speed and direction
 * @param  speed: -1000 to +1000 (negative = reverse)
 */
void Motor_SetLeft(int16_t speed);

/**
 * @brief  Set right motor speed and direction
 * @param  speed: -1000 to +1000 (negative = reverse)
 */
void Motor_SetRight(int16_t speed);

/**
 * @brief  Set both motors speed and direction
 * @param  left_speed: -1000 to +1000
 * @param  right_speed: -1000 to +1000
 */
void Motor_SetBoth(int16_t left_speed, int16_t right_speed);

/**
 * @brief  Stop both motors (brake)
 */
void Motor_Stop(void);

/**
 * @brief  Emergency stop (coast)
 */
void Motor_Coast(void);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_H */
