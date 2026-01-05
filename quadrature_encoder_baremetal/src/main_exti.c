/**
 ******************************************************************************
 * @file    main_exti.c
 * @brief   Example: Quadrature Encoder using EXTI
 *          Uses external interrupts instead of timer encoder mode
 ******************************************************************************
 */

#include "encoder.h"
#include "main.h"
#include <stdio.h>

/* Function prototypes */
void SystemClock_Config(void);
void GPIO_LED_Init(void);
void delay_ms(uint32_t ms);
void Error_Handler(void);

/* Global variables */
volatile uint32_t systick_counter = 0;

/**
 * @brief  Main program - EXTI Encoder Example
 */
int main(void) {
  int32_t encoder_count = 0;
  int32_t last_count = 0;

  /* HAL initialization */
  HAL_Init();

  /* Configure system clock */
  SystemClock_Config();

  /* Initialize LED */
  GPIO_LED_Init();

  /* Initialize encoder using EXTI */
  Encoder_EXTI_Init();

  char buffer[10];
  /* Main loop */
  while (1) {
    /* Read encoder count */
    encoder_count = Encoder_EXTI_GetCount();

    /* Toggle LED when count changes */
    if (encoder_count != last_count) {
      GPIOC->ODR ^= GPIO_ODR_ODR13;
      last_count = encoder_count;

      sprintf(buffer, "%d", last_count);
      USART_sendString(buffer);
    }

    delay_ms(10);
  }
}

/**
 * @brief  Configure system clock to 72MHz
 */
void SystemClock_Config(void) {
  SET_BIT(RCC->CR, RCC_CR_HSEON);
  while (!(RCC->CR & RCC_CR_HSERDY))
    ;

  MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2);
  SET_BIT(FLASH->ACR, FLASH_ACR_PRFTBE);

  MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL,
             RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9);

  SET_BIT(RCC->CR, RCC_CR_PLLON);
  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;

  MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    ;
}

/**
 * @brief  Initialize PC13 LED
 */
void GPIO_LED_Init(void) {
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPCEN);

  /* PC13 in CRH (bits 20-23): MODE=10 (2MHz output), CNF=00 (Push-Pull) */
  MODIFY_REG(GPIOC->CRH, 0x00F00000, (0x2 << 20));

  SET_BIT(GPIOC->ODR, GPIO_ODR_ODR13);
}

/**
 * @brief  Delay in milliseconds
 */
void delay_ms(uint32_t ms) {
  uint32_t start = systick_counter;
  while ((systick_counter - start) < ms)
    ;
}

/**
 * @brief  Error handler
 */
void Error_Handler(void) {
  __disable_irq();
  while (1) {
  }
}
