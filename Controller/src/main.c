/**
 ******************************************************************************
 * @file    main.c
 * @brief   PID Controller for Differential Drive Robot
 *          Straight-line motion using IMU and encoders
 ******************************************************************************
 */

#include "main.h"
#include "differential_drive.h"
#include "encoder.h"
#include "imu.h"
#include "motor.h"

/* ================ Global Variables ================ */

volatile uint32_t systick_counter = 0;

/* Control loop timing */
static volatile uint8_t control_flag = 0;

/* ================ Function Prototypes ================ */

void SystemClock_Config(void);
void GPIO_LED_Init(void);

/* ================ Main Program ================ */

/**
 * @brief  Main function
 */
int main(void) {
  /* HAL initialization */
  HAL_Init();

  /* Configure system clock to 72MHz */
  SystemClock_Config();

  /* Initialize LED (PC13) */
  GPIO_LED_Init();

  /* Initialize differential drive controller */
  DifferentialDrive_Init();

  /* Calibrate sensors (robot must be stationary!) */
  /* Blink LED fast during calibration */
  for (int i = 0; i < 10; i++) {
    GPIOC->ODR ^= LED_PIN;
    delay_ms(100);
  }

  DifferentialDrive_Calibrate();

  /* Indicate calibration complete - LED on */
  SET_BIT(GPIOC->ODR, LED_PIN);
  delay_ms(500);
  CLEAR_BIT(GPIOC->ODR, LED_PIN);

  /* Set target speed (encoder counts per second) */
  /* Adjust this value based on your encoder resolution and desired speed */
  float target_speed = 500.0f; /* counts/second */

  DifferentialDrive_SetSpeed(target_speed);

  /* Control loop timing */
  float dt = (float)CONTROL_PERIOD_MS / 1000.0f; /* Convert to seconds */
  uint32_t last_time = systick_counter;

  /* Main control loop */
  while (1) {
    /* Check if control interval has elapsed */
    if ((systick_counter - last_time) >= CONTROL_PERIOD_MS) {
      last_time = systick_counter;

      /* Update control loop */
      DifferentialDrive_Update(dt);

      /* Toggle LED to show activity */
      GPIOC->ODR ^= LED_PIN;
    }
  }
}

/* ================ System Functions ================ */

/**
 * @brief  Configure system clock to 72MHz using HSE + PLL
 */
void SystemClock_Config(void) {
  /* Enable HSE (High Speed External oscillator) */
  SET_BIT(RCC->CR, RCC_CR_HSEON);
  while (!(RCC->CR & RCC_CR_HSERDY))
    ;

  /* Configure Flash latency for 72MHz */
  MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2);
  SET_BIT(FLASH->ACR, FLASH_ACR_PRFTBE); /* Enable prefetch buffer */

  /* Configure bus prescalers */
  /* AHB = SYSCLK, APB1 = SYSCLK/2 (max 36MHz), APB2 = SYSCLK */
  MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2,
             RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PPRE2_DIV1);

  /* Configure PLL: HSE * 9 = 8MHz * 9 = 72MHz */
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL,
             RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9);

  /* Enable PLL */
  SET_BIT(RCC->CR, RCC_CR_PLLON);
  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;

  /* Switch to PLL as system clock */
  MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    ;
}

/**
 * @brief  Initialize LED on PC13
 */
void GPIO_LED_Init(void) {
  /* Enable GPIOC clock */
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPCEN);

  /* PC13 in CRH (bits 20-23): MODE=10 (2MHz output), CNF=00 (Push-Pull) */
  MODIFY_REG(GPIOC->CRH, 0x00F00000, (0x2 << 20));

  /* LED off initially (LED is active low on Blue Pill) */
  SET_BIT(GPIOC->ODR, LED_PIN);
}

/**
 * @brief  Delay in milliseconds (uses SysTick)
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
  /* Disable interrupts */
  __disable_irq();

  /* Fast blink LED to indicate error */
  while (1) {
    GPIOC->ODR ^= LED_PIN;
    for (volatile int i = 0; i < 100000; i++)
      ;
  }
}
