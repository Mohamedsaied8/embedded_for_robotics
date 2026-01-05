/**
 ******************************************************************************
 * @file    encoder.h
 * @brief   Quadrature Encoder using External Interrupts (EXTI)
 *          Header file with function prototypes
 ******************************************************************************
 */

#ifndef ENCODER_H
#define ENCODER_H

#include "uart.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ================ EXTI-based Encoder Functions ================ */

/**
 * @brief  Initialize encoder using external interrupts
 * @note   Uses PA0 (EXTI0) for Channel A, PA1 (EXTI1) for Channel B
 */
void Encoder_EXTI_Init(void);

/**
 * @brief  Get current encoder count
 * @retval Encoder position (signed 32-bit)
 */
int32_t Encoder_EXTI_GetCount(void);

/**
 * @brief  Reset encoder count to zero
 */
void Encoder_EXTI_Reset(void);

/**
 * @brief  Set encoder count to specific value
 * @param  count: New counter value
 */
void Encoder_EXTI_SetCount(int32_t count);

/**
 * @brief  Get encoder direction
 * @retval 1 = CW, -1 = CCW, 0 = stopped
 */
int8_t Encoder_EXTI_GetDirection(void);

/* ================ Polling-based Encoder Functions ================ */

/**
 * @brief  Initialize encoder for polling mode (no interrupts)
 */
void Encoder_Polling_Init(void);

/**
 * @brief  Update encoder in polling mode
 * @note   Must be called frequently in main loop
 */
void Encoder_Polling_Update(void);

/**
 * @brief  Get count in polling mode
 * @retval Encoder position (signed 32-bit)
 */
int32_t Encoder_Polling_GetCount(void);

/* ================ Interrupt Handlers (for vector table) ================ */

/**
 * @brief  EXTI0 interrupt handler (Channel A)
 */
void EXTI0_IRQHandler(void);

/**
 * @brief  EXTI1 interrupt handler (Channel B)
 */
void EXTI1_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* ENCODER_H */
