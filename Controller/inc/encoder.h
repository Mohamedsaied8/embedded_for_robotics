/**
 ******************************************************************************
 * @file    encoder.h
 * @brief   Dual Quadrature Encoder Interface (Left/Right wheels)
 ******************************************************************************
 *
 * Hardware Setup:
 *   - Left Encoder A  -> PA1 (EXTI1)
 *   - Left Encoder B  -> PA2 (EXTI2)
 *   - Right Encoder A -> PB8 (EXTI8)
 *   - Right Encoder B -> PB9 (EXTI9)
 *
 * Both encoders use EXTI for edge detection with XOR-based direction.
 *
 ******************************************************************************
 */

#ifndef ENCODER_H
#define ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Encoder configuration */
#define ENCODER_CPR 1200 /* Counts per revolution (adjust for your encoder) */
#define WHEEL_DIAMETER_MM 65 /* Wheel diameter in mm */

/**
 * @brief  Initialize both encoders using external interrupts
 */
void Encoder_Init(void);

/**
 * @brief  Get left encoder count
 * @retval Signed 32-bit count value
 */
int32_t Encoder_GetCountLeft(void);

/**
 * @brief  Get right encoder count
 * @retval Signed 32-bit count value
 */
int32_t Encoder_GetCountRight(void);

/**
 * @brief  Get left wheel speed in counts per second
 * @param  dt: Time delta in seconds
 * @retval Speed in counts/second
 */
float Encoder_GetSpeedLeft(float dt);

/**
 * @brief  Get right wheel speed in counts per second
 * @param  dt: Time delta in seconds
 * @retval Speed in counts/second
 */
float Encoder_GetSpeedRight(float dt);

/**
 * @brief  Reset both encoder counts to zero
 */
void Encoder_Reset(void);

/**
 * @brief  Reset left encoder count
 */
void Encoder_ResetLeft(void);

/**
 * @brief  Reset right encoder count
 */
void Encoder_ResetRight(void);

/**
 * @brief  Get delta counts since last call (for speed calculation)
 * @param  left_delta: Pointer to store left delta
 * @param  right_delta: Pointer to store right delta
 */
void Encoder_GetDelta(int32_t *left_delta, int32_t *right_delta);

/* Interrupt handlers (called from stm32f1xx_it.c) */
void Encoder_EXTI1_Handler(void);   /* Left A */
void Encoder_EXTI2_Handler(void);   /* Left B */
void Encoder_EXTI9_5_Handler(void); /* Right A/B (EXTI8, EXTI9) */

#ifdef __cplusplus
}
#endif

#endif /* ENCODER_H */
