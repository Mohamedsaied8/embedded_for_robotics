/**
 ******************************************************************************
 * @file    encoder.c
 * @brief   Dual Quadrature Encoder Implementation
 ******************************************************************************
 *
 * Hardware Setup:
 *   - Left Encoder A  -> PA1 (EXTI1)
 *   - Left Encoder B  -> PA2 (EXTI2)
 *   - Right Encoder A -> PB8 (EXTI8)
 *   - Right Encoder B -> PB9 (EXTI9)
 *
 ******************************************************************************
 */

#include "encoder.h"
#include "main.h"

/* ================ Private Variables ================ */

/* Encoder counters */
static volatile int32_t left_count = 0;
static volatile int32_t right_count = 0;

/* Last states for direction detection */
static volatile uint8_t left_last_A = 0;
static volatile uint8_t left_last_B = 0;
static volatile uint8_t right_last_A = 0;
static volatile uint8_t right_last_B = 0;

/* Previous counts for speed calculation */
static int32_t left_prev_count = 0;
static int32_t right_prev_count = 0;

/* ================ Private Functions ================ */

/**
 * @brief  Process left encoder state change
 */
static void ProcessLeftEncoder(void) {
  uint8_t A = (GPIOA->IDR >> 1) & 1; /* PA1 */
  uint8_t B = (GPIOA->IDR >> 2) & 1; /* PA2 */

  if (A != left_last_A || B != left_last_B) {
    if (A ^ left_last_B)
      left_count++;
    else
      left_count--;

    left_last_A = A;
    left_last_B = B;
  }
}

/**
 * @brief  Process right encoder state change
 */
static void ProcessRightEncoder(void) {
  uint8_t A = (GPIOB->IDR >> 8) & 1; /* PB8 */
  uint8_t B = (GPIOB->IDR >> 9) & 1; /* PB9 */

  if (A != right_last_A || B != right_last_B) {
    if (A ^ right_last_B)
      right_count++;
    else
      right_count--;

    right_last_A = A;
    right_last_B = B;
  }
}

/* ================ Public Functions ================ */

/**
 * @brief  Initialize both encoders using EXTI
 */
void Encoder_Init(void) {
  /* Enable clocks */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN); /* GPIOA */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN); /* GPIOB */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN); /* AFIO */

  /* Configure PA1, PA2 as Input with Pull-up (Left Encoder) */
  /* PA1: bits 4-7,  PA2: bits 8-11 in CRL */
  /* MODE=00 (Input), CNF=10 (Input with pull-up/down) = 0x8 */
  MODIFY_REG(GPIOA->CRL, 0x00000FF0, (0x8 << 4) | (0x8 << 8));
  SET_BIT(GPIOA->ODR, (1 << 1) | (1 << 2)); /* Enable pull-ups */

  /* Configure PB8, PB9 as Input with Pull-up (Right Encoder) */
  /* PB8: bits 0-3, PB9: bits 4-7 in CRH */
  MODIFY_REG(GPIOB->CRH, 0x000000FF, (0x8 << 0) | (0x8 << 4));
  SET_BIT(GPIOB->ODR, (1 << 8) | (1 << 9)); /* Enable pull-ups */

  /* Configure EXTI lines */
  /* EXTICR1: EXTI1[7:4]=0000 (PA1), EXTI2[11:8]=0000 (PA2) */
  MODIFY_REG(AFIO->EXTICR[0], 0x0FF0, 0x0000);

  /* EXTICR3: EXTI8[3:0]=0001 (PB8), EXTI9[7:4]=0001 (PB9) */
  MODIFY_REG(AFIO->EXTICR[2], 0x00FF, 0x0011);

  /* Enable rising and falling edge triggers */
  SET_BIT(EXTI->RTSR,
          EXTI_RTSR_TR1 | EXTI_RTSR_TR2 | EXTI_RTSR_TR8 | EXTI_RTSR_TR9);
  SET_BIT(EXTI->FTSR,
          EXTI_FTSR_TR1 | EXTI_FTSR_TR2 | EXTI_FTSR_TR8 | EXTI_FTSR_TR9);

  /* Clear pending interrupts */
  WRITE_REG(EXTI->PR, EXTI_PR_PR1 | EXTI_PR_PR2 | EXTI_PR_PR8 | EXTI_PR_PR9);

  /* Unmask EXTI interrupts */
  SET_BIT(EXTI->IMR, EXTI_IMR_MR1 | EXTI_IMR_MR2 | EXTI_IMR_MR8 | EXTI_IMR_MR9);

  /* Enable NVIC interrupts */
  NVIC_EnableIRQ(EXTI1_IRQn);
  NVIC_EnableIRQ(EXTI2_IRQn);
  NVIC_EnableIRQ(EXTI9_5_IRQn);

  /* Initialize last states */
  left_last_A = (GPIOA->IDR >> 1) & 1;
  left_last_B = (GPIOA->IDR >> 2) & 1;
  right_last_A = (GPIOB->IDR >> 8) & 1;
  right_last_B = (GPIOB->IDR >> 9) & 1;

  /* Reset counters */
  left_count = 0;
  right_count = 0;
  left_prev_count = 0;
  right_prev_count = 0;
}

/**
 * @brief  Get left encoder count
 */
int32_t Encoder_GetCountLeft(void) { return left_count; }

/**
 * @brief  Get right encoder count
 */
int32_t Encoder_GetCountRight(void) { return right_count; }

/**
 * @brief  Get left wheel speed
 */
float Encoder_GetSpeedLeft(float dt) {
  if (dt <= 0.0f)
    return 0.0f;

  int32_t current = left_count;
  int32_t delta = current - left_prev_count;
  left_prev_count = current;

  return (float)delta / dt;
}

/**
 * @brief  Get right wheel speed
 */
float Encoder_GetSpeedRight(float dt) {
  if (dt <= 0.0f)
    return 0.0f;

  int32_t current = right_count;
  int32_t delta = current - right_prev_count;
  right_prev_count = current;

  return (float)delta / dt;
}

/**
 * @brief  Reset both encoder counts
 */
void Encoder_Reset(void) {
  left_count = 0;
  right_count = 0;
  left_prev_count = 0;
  right_prev_count = 0;
}

/**
 * @brief  Reset left encoder
 */
void Encoder_ResetLeft(void) {
  left_count = 0;
  left_prev_count = 0;
}

/**
 * @brief  Reset right encoder
 */
void Encoder_ResetRight(void) {
  right_count = 0;
  right_prev_count = 0;
}

/**
 * @brief  Get delta counts
 */
void Encoder_GetDelta(int32_t *left_delta, int32_t *right_delta) {
  int32_t left_current = left_count;
  int32_t right_current = right_count;

  *left_delta = left_current - left_prev_count;
  *right_delta = right_current - right_prev_count;

  left_prev_count = left_current;
  right_prev_count = right_current;
}

/* ================ Interrupt Handlers ================ */

/**
 * @brief  EXTI1 handler (Left Encoder A - PA1)
 */
void Encoder_EXTI1_Handler(void) { ProcessLeftEncoder(); }

/**
 * @brief  EXTI2 handler (Left Encoder B - PA2)
 */
void Encoder_EXTI2_Handler(void) { ProcessLeftEncoder(); }

/**
 * @brief  EXTI9_5 handler (Right Encoder A/B - PB8/PB9)
 */
void Encoder_EXTI9_5_Handler(void) { ProcessRightEncoder(); }
