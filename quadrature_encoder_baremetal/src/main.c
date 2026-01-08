/**
 ******************************************************************************
 * @file    main.c
 * @brief   Quadrature Encoder using TIM2 with Motor Control
 *          Uses CMSIS register access
 ******************************************************************************
 * Hardware Setup:
 *   - Encoder Channel A -> PA0 (TIM2_CH1)
 *   - Encoder Channel B -> PA1 (TIM2_CH2)
 *   - LED (optional)    -> PC13 (onboard LED)
 *   - Motor PWM         -> PA6 (TIM3_CH1)
 *   - Motor IN1         -> PA4 (GPIO - direction)
 *   - Motor IN2         -> PA5 (GPIO - direction)
 *   - Push Button       -> PB4 (Active Low, internal pull-up)
 *
 * Encoder Mode 3: Counts on both TI1 and TI2 edges
 * Motor Control: Button pressed = CW, Button released = Stop
 ******************************************************************************
 */

#include "main.h"
#include "motor.h"
#include "uart.h"
#include <stdio.h>

/* Function prototypes */
void SystemClock_Config(void);
void GPIO_Init(void);
void Encoder_Init(void);
int32_t Encoder_GetCount(void);
void Encoder_Reset(void);
void delay_ms(uint32_t ms);
void Error_Handler(void);

/* Global variables */
volatile uint32_t systick_counter = 0;

/**
 * @brief  Main program
 */
int main(void) {
  int32_t encoder_count = 0;
  int32_t last_count = 0;
  char buffer[10];

  /* Configure system clock */
  SystemClock_Config();

  /* Initialize peripherals */
  GPIO_Init();
  Encoder_Init();

  /* Initialize motor and button */
  Motor_Init();
  // Button_Init();
  Uart1Init(); /* Initialize UART for serial output */
  RCC->APB2ENR = (1 << 0) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 11);
  GPIOA->CRL = 0x33333333;
  /* Main loop */
  while (1) {
    /* Check button state and control motor */
    // if (Button_IsPressed()) {
    /* Button pressed (active low) -> Motor CW at 80% speed */

    Motor_Run_CW(800);
    //} else {
    /* Button released -> Motor Stop */
    // Motor_Stop();
    // }

    /* Read encoder count */
    encoder_count = Encoder_GetCount();

    /* Check if count changed */
    if (encoder_count != last_count) {
      /* Toggle LED to show activity */
      GPIOC->ODR ^= GPIO_ODR_ODR13;
      last_count = encoder_count;
      sprintf(buffer, "%d", last_count);
      USART_sendString(buffer);
    }

    /* Small delay */
    delay_ms(10);
  }
}

/**
 * @brief  Configure system clock to 72MHz using HSE and PLL
 */
void SystemClock_Config(void) {
  /* Enable HSE (8 MHz external crystal) */
  SET_BIT(RCC->CR, RCC_CR_HSEON);

  /* Wait for HSE to be ready */
  while (!(RCC->CR & RCC_CR_HSERDY))
    ;
  /* Configure Flash latency (2 wait states for 72MHz) */
  MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2);
  SET_BIT(FLASH->ACR, FLASH_ACR_PRFTBE);

  /* Configure PLL: HSE * 9 = 72 MHz */
  /* Set PLLSRC = HSE and PLLMULL = x9 */
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL,
             RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9);

  /* Enable PLL */
  SET_BIT(RCC->CR, RCC_CR_PLLON);

  /* Wait for PLL to be ready */
  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;

  /* Select PLL as system clock */
  MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);

  /* Wait until PLL is used as system clock */
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    ;
}

/**
 * @brief  Initialize GPIO pins
 */
void GPIO_Init(void) {
  /* Enable GPIO clocks */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN); /* GPIOA for encoder */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPCEN); /* GPIOC for LED */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN); /* Alternate function */

  /* Configure PA0 and PA1 as floating input (for TIM2_CH1 and TIM2_CH2) */
  /* MODE=00 (Input), CNF=01 (Floating input) */
  /* PA0: bits 0-3, PA1: bits 4-7 */
  MODIFY_REG(GPIOA->CRL, 0x000000FF,
             (0x4 << 0) |     /* PA0: CNF=01, MODE=00 = 0x4 */
                 (0x4 << 4)); /* PA1: CNF=01, MODE=00 = 0x4 */

  /* Configure PC13 as output (LED) */
  /* PC13 is in CRH (bits 20-23): MODE=10 (2MHz output), CNF=00 (Push-Pull) */
  MODIFY_REG(GPIOC->CRH, 0x00F00000, (0x2 << 20));

  /* Set LED high (off for active-low LED) */
  SET_BIT(GPIOC->ODR, GPIO_ODR_ODR13);
}

/**
 * @brief  Initialize TIM2 in Encoder Mode
 */
void Encoder_Init(void) {
  /* Enable TIM2 clock */
  SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);

  /* Disable timer during configuration */
  CLEAR_BIT(TIM2->CR1, TIM_CR1_CEN);

  /* Configure TIM2 for Encoder Mode 3 (counts on both TI1 and TI2 edges) */
  /* SMS[2:0] = 011: Encoder mode 3 */
  MODIFY_REG(TIM2->SMCR, TIM_SMCR_SMS, TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1);

  /* Configure input capture channels */
  /* CC1S = 01: CC1 channel is input, IC1 mapped on TI1 */
  /* CC2S = 01: CC2 channel is input, IC2 mapped on TI2 */
  MODIFY_REG(TIM2->CCMR1, TIM_CCMR1_CC1S | TIM_CCMR1_CC2S,
             TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0);

  /* Configure polarity */
  /* CC1P = 0: non-inverted/rising edge */
  /* CC2P = 0: non-inverted/rising edge */
  /* Enable capture/compare channels */
  MODIFY_REG(TIM2->CCER, 0x00FF, TIM_CCER_CC1E | TIM_CCER_CC2E);

  /* Set auto-reload value to maximum */
  WRITE_REG(TIM2->ARR, 0xFFFFFFFF);

  /* Set prescaler to 0 (no prescaling) */
  WRITE_REG(TIM2->PSC, 0);

  /* Reset counter */
  WRITE_REG(TIM2->CNT, 0);

  /* Generate update event to reload prescaler */
  SET_BIT(TIM2->EGR, TIM_EGR_UG);

  /* Enable timer */
  SET_BIT(TIM2->CR1, TIM_CR1_CEN);
}

/**
 * @brief  Get current encoder count
 * @retval Encoder position (signed 32-bit)
 */
int32_t Encoder_GetCount(void) { return (int32_t)READ_REG(TIM2->CNT); }

/**
 * @brief  Reset encoder count to zero
 */
void Encoder_Reset(void) { WRITE_REG(TIM2->CNT, 0); }

/**
 * @brief  Simple delay function (milliseconds)
 * @param  ms: delay in milliseconds
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
