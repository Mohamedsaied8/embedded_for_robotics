/**
  ******************************************************************************
  * @file           : main.cpp
  * @brief          : OLED Display Demo Application
  * @description    : Demonstrates OOP approach for SSD1306 OLED display
  *                   on STM32F103 using I2C interface.
  ******************************************************************************
  */

#include "main.h"
#include "i2c.hpp"
#include "ssd1306.hpp"

/* Forward declarations */
extern "C" void SystemClock_Config(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* Initialize HAL */
    HAL_Init();
    
    /* Configure the system clock */
    SystemClock_Config();
    
    /* Create I2C object for OLED display */
    I2C i2c(OLED_I2C_ADDR);
    i2c.init();
    
    /* Create SSD1306 display object */
    SSD1306 display(i2c);
    display.init();
    
    /* Clear display */
    display.clear(Color::Black);
    
    /* Draw "Hello OLED!" title */
    display.drawString(20, 5, "Hello OLED!", Color::White);
    
    /* Draw a rectangle border */
    display.drawRect(0, 0, SSD1306_WIDTH, SSD1306_HEIGHT, Color::White);
    
    /* Draw a horizontal line */
    display.drawLine(0, 20, SSD1306_WIDTH - 1, 20, Color::White);
    
    /* Draw some info text */
    display.drawString(5, 25, "STM32F103 OOP Demo", Color::White);
    display.drawString(5, 35, "I2C: PB6/PB7", Color::White);
    display.drawString(5, 45, "Addr: 0x3C", Color::White);
    
    /* Draw a small filled rectangle */
    display.fillRect(100, 45, 20, 15, Color::White);
    
    /* Update display */
    display.display();
    
    /* Main loop */
    while (1)
    {
        /* Animation: toggle display inversion every second */
        static bool inverted = false;
        static uint32_t lastTick = 0;
        
        if (HAL_GetTick() - lastTick >= 2000)
        {
            lastTick = HAL_GetTick();
            inverted = !inverted;
            display.invertDisplay(inverted);
        }
        
        HAL_Delay(100);
    }
    
    return 0;
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
extern "C" void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Initializes the RCC Oscillators */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;  // 8MHz * 9 = 72MHz
    
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /* Initializes the CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  // 36MHz max
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  // 72MHz

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
extern "C" void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
