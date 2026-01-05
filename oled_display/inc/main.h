/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.cpp file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void SystemClock_Config(void);

/* Pin definitions for I2C1 --------------------------------------------------*/
#define I2C1_SCL_PIN    GPIO_PIN_6
#define I2C1_SDA_PIN    GPIO_PIN_7
#define I2C1_GPIO_PORT  GPIOB

/* OLED I2C Address ----------------------------------------------------------*/
#define OLED_I2C_ADDR   0x3C

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
