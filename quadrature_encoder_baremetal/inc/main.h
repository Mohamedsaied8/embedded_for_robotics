/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#ifndef STM32F103xB
#define STM32F103xB
#endif
#include "stm32f1xx_hal.h"

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Exported variables --------------------------------------------------------*/
extern volatile uint32_t systick_counter;

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
