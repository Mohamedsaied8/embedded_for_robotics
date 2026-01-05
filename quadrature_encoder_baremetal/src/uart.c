#include "uart.h"

#define FOSC 8000000
#define BAUD 9600

char USART_ReadChar() {
  while (!(USART1->SR & USART_SR_RXNE))
    ;
  return USART1->DR & 0xFF;
}

void USART_sendChar(char data) {
  USART1->DR = data & 0xFF;
  while (!(USART1->SR & USART_SR_TXE));
}

void USART_sendString(char *data) {
  while (*data != '\0') {
    USART_sendChar(*data);
    data++;
  }
}

void Uart1Init() {
  // Enable AFIO, GPIOA and USART1
  SET_BIT(RCC->APB2ENR,
          RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN);

  // PA9 to be Output alternate function push-pull for TX
  // PA10 to be input floating for RX
  MODIFY_REG(GPIOA->CRH, 0x00000FF0,
             (0xB << 4) |     /* PA9: MODE=11, CNF=10 (AF push-pull, 50MHz) */
                 (0x4 << 8)); /* PA10: MODE=00, CNF=01 (Input floating) */

  double usart_div = FOSC / (BAUD);
  USART1->BRR = (uint32_t)usart_div;

  // Enable Transmission and Reception
  USART1->CR1 = USART_CR1_RE | USART_CR1_TE;
  SET_BIT(USART1->CR1, USART_CR1_UE);
}