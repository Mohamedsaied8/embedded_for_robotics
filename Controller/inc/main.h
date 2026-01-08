/**
 ******************************************************************************
 * @file    main.h
 * @brief   Main header file - Common includes and definitions
 ******************************************************************************
 */

#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include <stdint.h>

/* System clock frequency */
#define SYSTEM_CLOCK_HZ 72000000U
#define CONTROL_LOOP_HZ 100U
#define CONTROL_PERIOD_MS (1000U / CONTROL_LOOP_HZ)

/* LED pin (Blue Pill onboard LED) */
#define LED_PIN GPIO_ODR_ODR13
#define LED_PORT GPIOC

/* Function prototypes */
void SystemClock_Config(void);
void Error_Handler(void);
void delay_ms(uint32_t ms);

/* Global variables */
extern volatile uint32_t systick_counter;

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
