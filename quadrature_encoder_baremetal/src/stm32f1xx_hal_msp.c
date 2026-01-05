/**
 ******************************************************************************
 * @file    stm32f1xx_hal_msp.c
 * @brief   This file provides code for the MSP Initialization
 *          and de-Initialization codes.
 ******************************************************************************
 */

#include "main.h"

/**
 * Initializes the Global MSP.
 */
void HAL_MspInit(void) {
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/
  /* Disable JTAG to free up pins */
  __HAL_AFIO_REMAP_SWJ_NOJTAG();
}
