/**
 ******************************************************************************
 * @file    stm32f1xx_hal_msp.c
 * @brief   HAL MSP (MCU Support Package) initialization
 ******************************************************************************
 */

#include "main.h"

/**
 * @brief  Initialize low-level hardware (called by HAL_Init)
 */
void HAL_MspInit(void) {
  /* Configure NVIC priority grouping */
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt priority grouping */
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* Configure SysTick for 1ms interrupt */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* Set SysTick interrupt priority */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}
