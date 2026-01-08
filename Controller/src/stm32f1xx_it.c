/**
 ******************************************************************************
 * @file    stm32f1xx_it.c
 * @brief   Interrupt handlers for Controller project
 ******************************************************************************
 */

#include "encoder.h"
#include "main.h"

/* External variables */
extern volatile uint32_t systick_counter;

/* ================ Cortex-M3 Exception Handlers ================ */

/**
 * @brief  NMI Handler
 */
void NMI_Handler(void) {
  while (1) {
  }
}

/**
 * @brief  Hard Fault Handler
 */
void HardFault_Handler(void) {
  while (1) {
  }
}

/**
 * @brief  Memory Manage Handler
 */
void MemManage_Handler(void) {
  while (1) {
  }
}

/**
 * @brief  Bus Fault Handler
 */
void BusFault_Handler(void) {
  while (1) {
  }
}

/**
 * @brief  Usage Fault Handler
 */
void UsageFault_Handler(void) {
  while (1) {
  }
}

/**
 * @brief  SVC Handler
 */
void SVC_Handler(void) {}

/**
 * @brief  Debug Monitor Handler
 */
void DebugMon_Handler(void) {}

/**
 * @brief  PendSV Handler
 */
void PendSV_Handler(void) {}

/**
 * @brief  SysTick Handler - 1ms tick
 */
void SysTick_Handler(void) {
  HAL_IncTick();
  systick_counter++;
}

/* ================ Peripheral Interrupt Handlers ================ */

/**
 * @brief  EXTI1 interrupt handler - Left Encoder Channel A (PA1)
 */
void EXTI1_IRQHandler(void) {
  if (EXTI->PR & EXTI_PR_PR1) {
    EXTI->PR = EXTI_PR_PR1; /* Clear pending bit */
    Encoder_EXTI1_Handler();
  }
}

/**
 * @brief  EXTI2 interrupt handler - Left Encoder Channel B (PA2)
 */
void EXTI2_IRQHandler(void) {
  if (EXTI->PR & EXTI_PR_PR2) {
    EXTI->PR = EXTI_PR_PR2; /* Clear pending bit */
    Encoder_EXTI2_Handler();
  }
}

/**
 * @brief  EXTI9_5 interrupt handler - Right Encoder (PB8, PB9)
 */
void EXTI9_5_IRQHandler(void) {
  /* Check EXTI8 (Right Encoder A) */
  if (EXTI->PR & EXTI_PR_PR8) {
    EXTI->PR = EXTI_PR_PR8; /* Clear pending bit */
    Encoder_EXTI9_5_Handler();
  }

  /* Check EXTI9 (Right Encoder B) */
  if (EXTI->PR & EXTI_PR_PR9) {
    EXTI->PR = EXTI_PR_PR9; /* Clear pending bit */
    Encoder_EXTI9_5_Handler();
  }
}
