#include "main.h"

// write ISR = IRQ4 for EXTI4
void EXTI4_IRQHandler(void) {
  if (EXTI->PR & (1 << 4)) {
    GPIOC->ODR ^= (1 << 13);
    EXTI->PR = (1 << 4);
  }
}

int main(void) {
  // Enable clock gating for AFIO, GPIOA and GPIOB
  RCC->APB2ENR = (1 << 0) | (1 << 2) | (1 << 3) | (1 << 4);

  // configure PA1 : LED to be output pin
  GPIOC->CRH = (3 << 20);

  // configure PB4 : button to be input pin
  GPIOB->CRL = (4 << 16);

  // set AFIO pin and PORT
  AFIO->EXTICR[1] = 0x0001;
  // Unmask EXT4 from IMR & EMR
  EXTI->IMR = (1 << 4);
  EXTI->EMR = (1 << 4);

  // configure EXT4 to be rising edge trigger
  EXTI->RTSR = (1 << 4);

  // Enable Interrupt for EXTI4 from NVIC
  NVIC->ISER[0] = (1 << 10);
  GPIOC->ODR |= (1 << 13);
  for (int i = 0; i < 1000000; i++)
    ;
  while (1) {
  }
  return 0;
}