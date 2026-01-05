/**
 ******************************************************************************
 * @file    encoder.c
 * @brief   Quadrature Encoder using External Interrupts (EXTI)
 *          Uses CMSIS register definitions
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

#include "encoder.h"
#include "main.h"

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
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN); /* GPIOA */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN); /* AFIO for EXTI */

  /* Configure PA0 and PA1 as input with pull-up */
  /* MODE=00 (Input), CNF=10 (Input with pull-up/down) */
  /* PA0: bits 0-3, PA1: bits 4-7 */
  // MODIFY_REG(GPIOA->CRL, 0x000000FF,
  //            (0x8 << 0) |     /* PA0: CNF=10, MODE=00 = 0x8 */
  //                (0x8 << 4)); /* PA1: CNF=10, MODE=00 = 0x8 */

  GPIOA->CRL = (0x8 << 0) |   /* PA0: CNF=10, MODE=00 = 0x8 */
                 (0x8 << 4); /* PA1: CNF=10, MODE=00 = 0x8 */

  /* Enable pull-ups on PA0 and PA1 */
  SET_BIT(GPIOA->ODR, 0 | 1);

  /* Configure EXTI0 and EXTI1 to use PA0 and PA1 */
  /* EXTICR1: EXTI0 = 0000 (PA0), EXTI1 = 0000 (PA1) */
  AFIO->EXTICR[0] = 0x0000;

  /* Enable rising and falling edge triggers for EXTI0 and EXTI1 */
  SET_BIT(EXTI->RTSR, EXTI_RTSR_TR0 | EXTI_RTSR_TR1); /* Rising edge */
  SET_BIT(EXTI->FTSR, EXTI_FTSR_TR0 | EXTI_FTSR_TR1); /* Falling edge */

  /* Clear any pending interrupts */
  WRITE_REG(EXTI->PR, EXTI_PR_PR0 | EXTI_PR_PR1);

  /* Unmask EXTI0 and EXTI1 interrupts */
  SET_BIT(EXTI->IMR, EXTI_IMR_MR0 | EXTI_IMR_MR1);

  /* Enable EXTI0 and EXTI1 in NVIC */
  NVIC_EnableIRQ(EXTI0_IRQn);
  NVIC_EnableIRQ(EXTI1_IRQn);

  /* Initialize last state */
  last_A = (GPIOA->IDR >> 0) & 1;
  last_B = (GPIOA->IDR >> 1) & 1;

  /* Reset counter */
  encoder_count = 0;
}

/**
 * @brief  Get current encoder count
 * @param  None
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
  if (EXTI->PR & EXTI_PR_PR0) {
    /* Clear pending bit */
    EXTI->PR = EXTI_PR_PR0;

    /* Process encoder state */
    Encoder_ProcessState();
  }
}

/**
 * @brief  EXTI1 interrupt handler (Channel B edge)
 */
void EXTI1_IRQHandler(void) {
  /* Check if EXTI1 triggered */
  if (EXTI->PR & EXTI_PR_PR1) {
    /* Clear pending bit */
    EXTI->PR = EXTI_PR_PR1;

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
  MODIFY_REG(GPIOA->CRL, 0x000000FF,
             (0x8 << 0) |     /* PA0: CNF=10, MODE=00 */
                 (0x8 << 4)); /* PA1: CNF=10, MODE=00 */

  /* Enable pull-ups */
  SET_BIT(GPIOA->ODR, GPIO_ODR_ODR0 | GPIO_ODR_ODR1);

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
