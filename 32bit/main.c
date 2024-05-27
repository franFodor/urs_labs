#include <stm32f407xx.h>
#include <stdio.h>
#include "init.h"

void Delay(uint32_t NoOfTicks);

void Delay(uint32_t NoOfTicks) {
  uint32_t currentTicks = msTicks;
  while((msTicks - currentTicks) < NoOfTicks);
}

int main(void) {
  FILE* fMorse;
  volatile uint32_t tmp;
  
  // enable clock for GPIOD (datasheet pg 9)
  RCC->AHB1ENR |= (0x1UL << 3);
  // enable clock for GPIOB
  RCC->AHB1ENR |= (0x1UL << 1);
  // enable clock for GPIOE
  RCC->AHB1ENR |= (0x1UL << 4);
  
  // dummy read - we need to wait for the value to get written
  tmp = RCC->AHB1ENR;
  
  // configure pins 12 and 13 (datasheet pg 10) (ex 5)
  GPIOD->MODER &= ~( (0x3UL << 24U) | (0x3UL << 26U) );
  GPIOD->MODER |=  ( (0x1UL << 24U) | (0x1UL << 26U) );
  
  // configure pins 13, 14, 15 (datasheet pg 10) (ex 6)
  GPIOB->MODER &= ~( (0x3UL << 26U) | (0x3UL << 28U) | (0x3UL << 30U) );
  GPIOB->MODER |=  ( (0x1UL << 26U) | (0x1UL << 28U) | (0x1UL << 30U) );
  
  // configure buzzer as output
  GPIOE->MODER &= ~( (0x3UL << 22U) );
  GPIOE->MODER |=  ( (0x1UL << 22U) );
  
  // turn off led H4, turn on led H3 (datasheet pg 11) (ex 5)
  GPIOD->ODR   &= ~( (0x1UL << 12U) );
  GPIOD->ODR   |= ~( (0x1UL << 13U) );
  
  // turn on rgb for 1 second each (ex 6)
  for(uint8_t i = 0; i < 1; i++) {
    // mask everything with one, set 0 to led pins
    GPIOB->ODR &= 0xFFFF1FFF;     // turn all leds off
    GPIOB->ODR |= (0x1UL << 13U); // turn red one on
    Delay(1000);
    GPIOB->ODR &= 0xFFFF1FFF;     // turn all leds off
    GPIOB->ODR |= (0x1UL << 14U); // turn green one on
    Delay(1000);
    GPIOB->ODR &= 0xFFFF1FFF;     // turn all leds off
    GPIOB->ODR |= (0x1UL << 15U); // turn blue one on
    Delay(1000);
  }
  
  GPIOB->ODR &= 0xFFFF1FFF;       // turn all leds off
  
  printf("%s", "Redefinicija sistemskih poziva uspjesno napravljena !\r\n"); 
  
  fMorse = fopen("Morse","w"); 
  fprintf(fMorse,"SOS - SOS - PURS"); 
  fclose(fMorse); 
  
  while(1);
}
