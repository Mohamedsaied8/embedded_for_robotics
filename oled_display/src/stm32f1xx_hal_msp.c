/**
  ******************************************************************************
  * @file    stm32f1xx_hal_msp.c
  * @brief   HAL MSP (MCU Support Package) initialization functions.
  ******************************************************************************
  */

#include "main.h"

/**
  * @brief  Initialize the Global MSP.
  */
void HAL_MspInit(void)
{
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    /* System interrupt init */
    /* NOJTAG: JTAG-DP Disabled and SW-DP Enabled */
    __HAL_AFIO_REMAP_SWJ_NOJTAG();
}

/**
  * @brief  DeInitializes the Global MSP.
  */
void HAL_MspDeInit(void)
{
}
