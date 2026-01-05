/**
 ******************************************************************************
 * @file    encoder.c
 * @brief   Quadrature Encoder using External Interrupts (EXTI)
 *          Bare-metal implementation - No CMSIS, No HAL
 ******************************************************************************
 *
 * This implementation uses external interrupts instead of timer encoder mode.
 * Useful when timer encoder pins are not available or for learning purposes.
 *
 * Hardware Setup:
 *   - Encoder Channel A -> PA0 (EXTI0)
 *   - Encoder Channel B -> PA1 (EXTI1)
 *
 * How it works:
 *   - EXTI0 triggers on both rising and falling edges of Channel A
 *   - EXTI1 triggers on both rising and falling edges of Channel B
 *   - Direction is determined by comparing A and B states
 *   - Counter is incremented/decremented in ISR
 *
 ******************************************************************************
 */

#include <stdint.h>

#include "encoder.h"
#include "stm32f1xx_hal.h"

#define EXTI0_IRQn 6
#define EXTI1_IRQn 7
#define GPIO_CNF_INPUT_PUPD 0x02
#define GPIO_MODE_INPUT 0x00
/* ================ Additional Register Definitions ================ */

/* ================ Encoder State ================ */

/* Encoder counter (signed for bidirectional counting) */
static volatile int32_t encoder_count = 0;

/* Last known states of A and B channels */
static volatile uint8_t last_A = 0;
static volatile uint8_t last_B = 0;

/* ================ Private Functions ================ */

/**
 * @brief  Process encoder state change using XOR method
 * @note   Simplified decoding: direction = A XOR last_B
 *         - If (A XOR last_B) = 1: Clockwise
 *         - If (A XOR last_B) = 0: Counter-clockwise
 */
static void Encoder_ProcessState(void) {
  uint8_t A = (GPIOA->IDR >> 0) & 1; /* Read PA0 */
  uint8_t B = (GPIOA->IDR >> 1) & 1; /* Read PA1 */

  /* Only process if state actually changed */
  if (A != last_A || B != last_B) {
    /* XOR method for direction detection */
    if (A ^ last_B)
      encoder_count++; /* Clockwise */
    else
      encoder_count--; /* Counter-clockwise */

    last_A = A;
    last_B = B;
  }
}

/* ================ Public Functions ================ */

/**
 * @brief  Initialize encoder using external interrupts
 */
void Encoder_EXTI_Init(void) {
  /* Enable clocks */
  SET_BIT(RCC->APB2ENR, 2); /* GPIOA */
  SET_BIT(RCC->APB2ENR, 0); /* AFIO for EXTI */

  /* Configure PA0 and PA1 as input with pull-up */
  /* PA0: CNF=10 (pull-up/down), MODE=00 (input) */
  /* PA1: CNF=10 (pull-up/down), MODE=00 (input) */
  GPIOA->CRL = (0x02 << 2) | (0x00 << 0) | (0x02 << 6) | (0x00 << 4);

  /* Enable pull-ups on PA0 and PA1 */
  SET_BIT(GPIOA->ODR, (1 << 0) | (1 << 1));

  /* Configure EXTI0 and EXTI1 to use PA0 and PA1 */
  /* EXTICR1: EXTI0 = 0000 (PA0), EXTI1 = 0000 (PA1) */
  AFIO->EXTICR[0] = 0x0000;

  /* Enable rising and falling edge triggers for EXTI0 and EXTI1 */
  SET_BIT(EXTI->RTSR, (1 << 0) | (1 << 1)); /* Rising edge */
  SET_BIT(EXTI->FTSR, (1 << 0) | (1 << 1)); /* Falling edge */

  /* Clear any pending interrupts */
  EXTI->PR = (1 << 0) | (1 << 1);

  /* Unmask EXTI0 and EXTI1 interrupts */
  SET_BIT(EXTI->IMR, (1 << 0) | (1 << 1));

  /* Enable EXTI0 and EXTI1 in NVIC */
  SET_BIT(NVIC->ISER[0], (1 << EXTI0_IRQn));
  SET_BIT(NVIC->ISER[0], (1 << EXTI1_IRQn));

  /* Initialize last state */
  last_A = (GPIOA->IDR >> 0) & 1;
  last_B = (GPIOA->IDR >> 1) & 1;

  /* Reset counter */
  encoder_count = 0;
}

/**
 * @brief  Get current encoder count
 * @retval Encoder position (signed 32-bit)
 */
int32_t Encoder_EXTI_GetCount(void) { return encoder_count; }

/**
 * @brief  Reset encoder count to zero
 */
void Encoder_EXTI_Reset(void) { encoder_count = 0; }

/**
 * @brief  Set encoder count to specific value
 * @param  count: New counter value
 */
void Encoder_EXTI_SetCount(int32_t count) { encoder_count = count; }

/**
 * @brief  Get encoder direction
 * @retval 1 = CW, -1 = CCW, 0 = stopped
 */
int8_t Encoder_EXTI_GetDirection(void) {
  static int32_t prev_count = 0;
  int32_t current = encoder_count;
  int8_t dir = 0;

  if (current > prev_count)
    dir = 1;
  else if (current < prev_count)
    dir = -1;

  prev_count = current;
  return dir;
}

/* ================ Interrupt Handlers ================ */

/**
 * @brief  EXTI0 interrupt handler (Channel A edge)
 */
void EXTI0_IRQHandler(void) {
  /* Check if EXTI0 triggered */
  if (EXTI->PR & (1 << 0)) {
    /* Clear pending bit */
    EXTI->PR = (1 << 0);

    /* Process encoder state */
    Encoder_ProcessState();
  }
}

/**
 * @brief  EXTI1 interrupt handler (Channel B edge)
 */
void EXTI1_IRQHandler(void) {
  /* Check if EXTI1 triggered */
  if (EXTI->PR & (1 << 1)) {
    /* Clear pending bit */
    EXTI->PR = (1 << 1);

    /* Process encoder state */
    Encoder_ProcessState();
  }
}

/* ================ Alternative: Polling Mode ================ */

/**
 * @brief  Initialize encoder for polling mode (no interrupts)
 */
void Encoder_Polling_Init(void) {
  /* Enable GPIOA clock */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);

  /* Configure PA0 and PA1 as input with pull-up */
  GPIOA->CRL = (GPIO_CNF_INPUT_PUPD << 2) | (GPIO_MODE_INPUT << 0) |
               (GPIO_CNF_INPUT_PUPD << 6) | (GPIO_MODE_INPUT << 4);

  /* Enable pull-ups */
  SET_BIT(GPIOA->ODR, (1 << 0) | (1 << 1));

  /* Initialize state */
  last_A = (GPIOA->IDR >> 0) & 1;
  last_B = (GPIOA->IDR >> 1) & 1;
  encoder_count = 0;
}

/**
 * @brief  Update encoder in polling mode
 * @note   Call this function frequently in main loop
 */
void Encoder_Polling_Update(void) { Encoder_ProcessState(); }

/**
 * @brief  Get count in polling mode (same as EXTI)
 */
int32_t Encoder_Polling_GetCount(void) { return encoder_count; }
