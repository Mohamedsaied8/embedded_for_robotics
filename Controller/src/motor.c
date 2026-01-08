/**
 ******************************************************************************
 * @file    motor.c
 * @brief   Dual DC Motor Control using TIM3 PWM
 ******************************************************************************
 *
 * Hardware Setup:
 *   - Left Motor PWM   -> PA6 (TIM3_CH1)
 *   - Left Motor IN1   -> PA4 (GPIO - direction)
 *   - Left Motor IN2   -> PA5 (GPIO - direction)
 *   - Right Motor PWM  -> PA7 (TIM3_CH2)
 *   - Right Motor IN1  -> PB0 (GPIO - direction)
 *   - Right Motor IN2  -> PB1 (GPIO - direction)
 *
 ******************************************************************************
 */

#include "motor.h"

/* ================ Private Defines ================ */

#define MOTOR_PWM_PERIOD 1000 /* PWM period for 0-100% */

/* Left motor direction pins */
#define LEFT_IN1_PIN GPIO_ODR_ODR4
#define LEFT_IN2_PIN GPIO_ODR_ODR5
#define LEFT_PORT GPIOA

/* Right motor direction pins */
#define RIGHT_IN1_PIN GPIO_ODR_ODR0
#define RIGHT_IN2_PIN GPIO_ODR_ODR1
#define RIGHT_PORT GPIOB

/* ================ Private Functions ================ */

/**
 * @brief  Clamp speed to valid range
 */
static int16_t clamp_speed(int16_t speed) {
  if (speed > MOTOR_MAX_SPEED)
    return MOTOR_MAX_SPEED;
  if (speed < MOTOR_MIN_SPEED)
    return MOTOR_MIN_SPEED;
  return speed;
}

/**
 * @brief  Get absolute value
 */
static uint16_t abs_speed(int16_t speed) {
  return (speed < 0) ? (uint16_t)(-speed) : (uint16_t)speed;
}

/* ================ Public Functions ================ */

/**
 * @brief  Initialize dual motor control
 */
void Motor_Init(void) {
  /* Enable clocks */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN); /* GPIOA */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN); /* GPIOB */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN); /* AFIO */
  SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN); /* TIM3 */

  /* Configure PA4, PA5 as GPIO Output (Left Motor Direction) */
  /* PA4: bits 16-19, PA5: bits 20-23 in CRL */
  /* MODE=10 (2MHz), CNF=00 (Push-Pull) = 0x2 */
  MODIFY_REG(GPIOA->CRL, 0x00FF0000, (0x2 << 16) | (0x2 << 20));

  /* Configure PA6, PA7 as Alternate Function Push-Pull (TIM3_CH1, TIM3_CH2) */
  /* PA6: bits 24-27, PA7: bits 28-31 in CRL */
  /* MODE=10 (2MHz), CNF=10 (AF Push-Pull) = 0xA */
  MODIFY_REG(GPIOA->CRL, 0xFF000000, (0xA << 24) | (0xA << 28));

  /* Configure PB0, PB1 as GPIO Output (Right Motor Direction) */
  /* PB0: bits 0-3, PB1: bits 4-7 in CRL */
  MODIFY_REG(GPIOB->CRL, 0x000000FF, (0x2 << 0) | (0x2 << 4));

  /* Initialize all direction pins low (motors stopped) */
  CLEAR_BIT(LEFT_PORT->ODR, LEFT_IN1_PIN | LEFT_IN2_PIN);
  CLEAR_BIT(RIGHT_PORT->ODR, RIGHT_IN1_PIN | RIGHT_IN2_PIN);

  /* Disable timer during configuration */
  CLEAR_BIT(TIM3->CR1, TIM_CR1_CEN);

  /* Set prescaler for 1kHz PWM at 72MHz */
  /* 72MHz / 72 / 1000 = 1kHz */
  WRITE_REG(TIM3->PSC, 71);
  WRITE_REG(TIM3->ARR, MOTOR_PWM_PERIOD - 1);

  /* Configure TIM3_CH1 (PA6) in PWM Mode 1 */
  MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_OC1M | TIM_CCMR1_OC1PE,
             TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE);

  /* Configure TIM3_CH2 (PA7) in PWM Mode 1 */
  MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_OC2M | TIM_CCMR1_OC2PE,
             TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE);

  /* Enable CH1 and CH2 outputs */
  SET_BIT(TIM3->CCER, TIM_CCER_CC1E | TIM_CCER_CC2E);

  /* Initialize duty cycles to 0 */
  WRITE_REG(TIM3->CCR1, 0);
  WRITE_REG(TIM3->CCR2, 0);

  /* Generate update event to load registers */
  SET_BIT(TIM3->EGR, TIM_EGR_UG);

  /* Enable auto-reload preload */
  SET_BIT(TIM3->CR1, TIM_CR1_ARPE);

  /* Enable timer */
  SET_BIT(TIM3->CR1, TIM_CR1_CEN);
}

/**
 * @brief  Set left motor speed and direction
 */
void Motor_SetLeft(int16_t speed) {
  speed = clamp_speed(speed);

  if (speed > 0) {
    /* Forward: IN1=HIGH, IN2=LOW */
    SET_BIT(LEFT_PORT->ODR, LEFT_IN1_PIN);
    CLEAR_BIT(LEFT_PORT->ODR, LEFT_IN2_PIN);
  } else if (speed < 0) {
    /* Reverse: IN1=LOW, IN2=HIGH */
    CLEAR_BIT(LEFT_PORT->ODR, LEFT_IN1_PIN);
    SET_BIT(LEFT_PORT->ODR, LEFT_IN2_PIN);
  } else {
    /* Stop: IN1=LOW, IN2=LOW (brake) */
    CLEAR_BIT(LEFT_PORT->ODR, LEFT_IN1_PIN | LEFT_IN2_PIN);
  }

  /* Set PWM duty cycle */
  WRITE_REG(TIM3->CCR1, abs_speed(speed));
}

/**
 * @brief  Set right motor speed and direction
 */
void Motor_SetRight(int16_t speed) {
  speed = clamp_speed(speed);

  if (speed > 0) {
    /* Forward: IN1=HIGH, IN2=LOW */
    SET_BIT(RIGHT_PORT->ODR, RIGHT_IN1_PIN);
    CLEAR_BIT(RIGHT_PORT->ODR, RIGHT_IN2_PIN);
  } else if (speed < 0) {
    /* Reverse: IN1=LOW, IN2=HIGH */
    CLEAR_BIT(RIGHT_PORT->ODR, RIGHT_IN1_PIN);
    SET_BIT(RIGHT_PORT->ODR, RIGHT_IN2_PIN);
  } else {
    /* Stop: IN1=LOW, IN2=LOW (brake) */
    CLEAR_BIT(RIGHT_PORT->ODR, RIGHT_IN1_PIN | RIGHT_IN2_PIN);
  }

  /* Set PWM duty cycle */
  WRITE_REG(TIM3->CCR2, abs_speed(speed));
}

/**
 * @brief  Set both motors
 */
void Motor_SetBoth(int16_t left_speed, int16_t right_speed) {
  Motor_SetLeft(left_speed);
  Motor_SetRight(right_speed);
}

/**
 * @brief  Stop both motors (brake)
 */
void Motor_Stop(void) {
  /* Set direction pins low */
  CLEAR_BIT(LEFT_PORT->ODR, LEFT_IN1_PIN | LEFT_IN2_PIN);
  CLEAR_BIT(RIGHT_PORT->ODR, RIGHT_IN1_PIN | RIGHT_IN2_PIN);

  /* Set PWM to 0 */
  WRITE_REG(TIM3->CCR1, 0);
  WRITE_REG(TIM3->CCR2, 0);
}

/**
 * @brief  Emergency stop (coast - let motors spin freely)
 */
void Motor_Coast(void) {
  /* Set both IN pins high (coast mode) */
  SET_BIT(LEFT_PORT->ODR, LEFT_IN1_PIN | LEFT_IN2_PIN);
  SET_BIT(RIGHT_PORT->ODR, RIGHT_IN1_PIN | RIGHT_IN2_PIN);

  /* PWM doesn't matter in coast mode */
  WRITE_REG(TIM3->CCR1, 0);
  WRITE_REG(TIM3->CCR2, 0);
}
