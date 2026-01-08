/**
 ******************************************************************************
 * @file    motor.h
 * @brief   DC Motor Control using TIM3 PWM
 *          Header file with function prototypes
 ******************************************************************************
 *
 * Hardware Setup:
 *   - Motor PWM     -> PA6 (TIM3_CH1)
 *   - Motor IN1     -> PA4 (GPIO - direction)
 *   - Motor IN2     -> PA5 (GPIO - direction)
 *   - Push Button   -> PB4 (Active Low, internal pull-up)
 *
 ******************************************************************************
 */

#ifndef MOTOR_H
#define MOTOR_H

#include "main.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ================ Motor Control Functions ================ */

/**
 * @brief  Initialize motor control (TIM3 PWM on PA6)
 */
void Motor_Init(void);

/**
 * @brief  Initialize push button on PB4 (input with pull-up)
 */
void Button_Init(void);

/**
 * @brief  Run motor clockwise at specified speed
 * @param  speed: PWM duty cycle (0-1000)
 */
void Motor_Run_CW(uint16_t speed);

/**
 * @brief  Run motor counter-clockwise at specified speed
 * @param  speed: PWM duty cycle (0-1000)
 */
void Motor_Run_CCW(uint16_t speed);

/**
 * @brief  Stop the motor
 */
void Motor_Stop(void);

/**
 * @brief  Set motor speed (PWM duty cycle)
 * @param  speed: 0-1000 (0% to 100%)
 */
void Motor_SetSpeed(uint16_t speed);

/**
 * @brief  Check if button is pressed
 * @retval 1 if pressed (active low), 0 if not pressed
 */
uint8_t Button_IsPressed(void);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_H */
