/**
 ******************************************************************************
 * @file    motor.c
 * @brief   DC Motor Control using TIM3 PWM
 *          Uses CMSIS register definitions
 ******************************************************************************
 *
 * This implementation uses TIM3 in PWM mode for motor speed control.
 *
 * Hardware Setup:
 *   - Motor PWM     -> PA6 (TIM3_CH1)
 *   - Motor IN1     -> PA4 (GPIO output - direction)
 *   - Motor IN2     -> PA5 (GPIO output - direction)
 *   - Push Button   -> PB4 (Active Low, internal pull-up)
 *
 * PWM Configuration:
 *   - Timer: TIM3
 *   - Frequency: 1kHz (adjustable via ARR)
 *   - Resolution: 0-1000 (10-bit like resolution)
 *
 ******************************************************************************
 */

#include "motor.h"

/* ================ Private Defines ================ */
#define MOTOR_PWM_PERIOD 1000 /* PWM period for 0-100% in 0.1% steps */

/* ================ Public Functions ================ */

/**
 * @brief  Initialize TIM3 for PWM motor control on PA6
 */
void Motor_Init(void) {
  /* Enable clocks */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN); /* GPIOA */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN); /* AFIO */
  SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN); /* TIM3 */

  /* Configure PA4, PA5 as GPIO Output Push-Pull (Direction control IN1, IN2) */
  /* PA4: bits 16-19 in CRL: MODE=10 (2MHz), CNF=00 (Push-Pull) = 0x2 */
  /* PA5: bits 20-23 in CRL: MODE=10 (2MHz), CNF=00 (Push-Pull) = 0x2 */
  GPIOA->CRL = (0x3 << 16) | (0x3 << 20);

  /* Configure PA6 as Alternate Function Push-Pull (TIM3_CH1) */
  /* PA6: bits 24-27 in CRL: MODE=10 (2MHz), CNF=10 (AF Push-Pull) = 0xA */
  MODIFY_REG(GPIOA->CRL, (0xF << 24), (0xA << 24));

  /* Set direction pins low initially (motor stopped) */
  CLEAR_BIT(GPIOA->ODR, GPIO_ODR_ODR4 | GPIO_ODR_ODR5);

  /* Disable timer during configuration */
  CLEAR_BIT(TIM3->CR1, TIM_CR1_CEN);

  /* Set prescaler for 1kHz PWM at 72MHz clock */
  /* PWM freq = 72MHz / (PSC+1) / (ARR+1) */
  /* 72MHz / 72 / 1000 = 1kHz */
  WRITE_REG(TIM3->PSC, 71);                   /* Prescaler = 72-1 */
  WRITE_REG(TIM3->ARR, MOTOR_PWM_PERIOD - 1); /* Period = 1000-1 */

  /* Configure TIM3_CH1 in PWM Mode 1 */
  /* OC1M = 110: PWM mode 1, OC1PE = 1: Preload enable */
  MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_OC1M | TIM_CCMR1_OC1PE,
             TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE);

  /* Enable CH1 output */
  SET_BIT(TIM3->CCER, TIM_CCER_CC1E);

  /* Initialize duty cycle to 0 (motor stopped) */
  WRITE_REG(TIM3->CCR1, 0);

  /* Generate update event to load prescaler */
  SET_BIT(TIM3->EGR, TIM_EGR_UG);

  /* Enable auto-reload preload */
  SET_BIT(TIM3->CR1, TIM_CR1_ARPE);

  /* Enable timer */
  SET_BIT(TIM3->CR1, TIM_CR1_CEN);
}

/**
 * @brief  Initialize push button on PB4 as input with internal pull-up
 */
void Button_Init(void) {
  /* Enable GPIOB clock */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);

  /* Configure PB4 as Input with Pull-up */
  /* PB4: bits 16-19 in CRL: MODE=00 (Input), CNF=10 (Pull-up/down) = 0x8 */
  MODIFY_REG(GPIOB->CRL, (0xF << 16), (0x8 << 16));

  /* Enable pull-up by setting ODR bit */
  SET_BIT(GPIOB->ODR, GPIO_ODR_ODR4);
}

/**
 * @brief  Check if button is pressed (active low)
 * @retval 1 if pressed, 0 if not pressed
 */
uint8_t Button_IsPressed(void) {
  /* Button is active low: pressed = 0, not pressed = 1 */
  return !(GPIOB->IDR & GPIO_IDR_IDR4);
}

/**
 * @brief  Run motor clockwise at specified speed
 * @param  speed: PWM duty cycle (0-1000)
 */
void Motor_Run_CW(uint16_t speed) {
  /* Set direction: IN1=HIGH, IN2=LOW for CW */
  SET_BIT(GPIOA->ODR, GPIO_ODR_ODR4);
  CLEAR_BIT(GPIOA->ODR, GPIO_ODR_ODR5);

  /* Set PWM duty cycle */
  Motor_SetSpeed(speed);
}

/**
 * @brief  Run motor counter-clockwise at specified speed
 * @param  speed: PWM duty cycle (0-1000)
 */
void Motor_Run_CCW(uint16_t speed) {
  /* Set direction: IN1=LOW, IN2=HIGH for CCW */
  CLEAR_BIT(GPIOA->ODR, GPIO_ODR_ODR4);
  SET_BIT(GPIOA->ODR, GPIO_ODR_ODR5);

  /* Set PWM duty cycle */
  Motor_SetSpeed(speed);
}

/**
 * @brief  Stop the motor
 */
void Motor_Stop(void) {
  /* Set both direction pins LOW (brake) */
  CLEAR_BIT(GPIOA->ODR, GPIO_ODR_ODR4 | GPIO_ODR_ODR5);
  /* Set PWM duty cycle to 0 */
  WRITE_REG(TIM3->CCR1, 0);
}

/**
 * @brief  Set motor speed (PWM duty cycle)
 * @param  speed: 0-1000 (0% to 100% in 0.1% steps)
 */
void Motor_SetSpeed(uint16_t speed) {
  /* Clamp speed to maximum */
  if (speed > MOTOR_PWM_PERIOD) {
    speed = MOTOR_PWM_PERIOD;
  }

  /* Set PWM duty cycle */
  WRITE_REG(TIM3->CCR1, speed);
}
