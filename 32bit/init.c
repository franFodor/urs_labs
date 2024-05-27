#include <stm32f407xx.h>
#include "init.h"

void init_DefaultClock(void) {
  // (datasheet pg 7)
  RCC->CR     |= (uint32_t)0x00000001;        // HSION = 1
  RCC->CFGR    = 0x00000000;                  // reset CFGR reg
  
  RCC->CR     &= (uint32_t)0xFEF6FFFF;        // HSEON, CSSON, PLLON = 0
  RCC->PLLCFGR = 0x24003010;                  // reset PLLCFGR register
  
  RCC->CR     &= (uint32_t)0xFFFBFFFF;        // HSEBYP = 0
  RCC->CIR     = 0x00000000;                  // disable all clock interrupts
}

void init_Clock(void) {
  // set the clock in the state which it is at after reset
  init_DefaultClock();
  
  // HSE OSCILLATOR (datasheet pg 7)
  RCC->CR     |= 0x1UL << 16;                 // HSEON = 1
  while( !(RCC->CR & (0x1UL << 17)) );        // while(HSREADY = 0)
  
  // PLL (PLLCLK = 100 MHz)
  RCC->PLLCFGR = 0x24416408;                  // PPL output (M = 8, N = 400, P = 4) => 100 MHz
  RCC->CR     |= 0x1UL << 24;                 // PLLON = 1
  while( !(RCC->CR & (0x1UL << 25)) );        // while(PLLREADY = 0)
      
  // clock for HCLK, APB1 presc. and APB2 presc. (datasheet pg 8)
  RCC->CFGR   |= 0x8UL << 4;                  // HPRE: divide by 2 (100 / 2 = 50 MHz)
  
  // maximum prescaller value for APB1 and APB2 (datasheet pg 8)
  RCC->CFGR   |= 0x7UL << 10;                 // PPRE1: divide by 16
  RCC->CFGR   |= 0x7UL << 13;                 // PPRE2: divide by 16
  
  // selection of PLLCLK as SYSCLK
  RCC->CFGR   |= 0x2UL;                       // SW = 2
  while( (RCC->CFGR & 0xCUL)>>2 != (RCC->CFGR & 0x3UL) );
                                              // while(SWS!=SW)
  // Desired frequencies for APB1 and APB2 clocks 
  RCC->CFGR   &= ~(0x7UL<<10) | (0x4UL<<10);  // PPRE1: div. 2
  RCC->CFGR   &= ~(0x7UL<<13) | (0x0UL<<13);  // PPRE2: div. 1
}

void init_FlashAccess(void) {
  //(datasheet pg 6)
  uint8_t WS = 1;                             // latency = 1

  // art init
  FLASH->ACR = WS 
               | (0x1UL<< 8)                  // FLASH_ACR_PRFTEN  
               | (0x1UL<< 9)                  // FLASH_ACR_ICEN 
               | (0x1UL<<10);                 // FLASH_ACR_DCEN 
  while( ((FLASH->ACR)&(0x7))!=WS );          // wait acception 
}

void init_SysTick(void) {
  // (datasheet pg 5)
  SCB->SHP[11]   = 0x2 << 4;                  // priotiy of SysTick
  SCB->AIRCR     = (SCB->AIRCR & 0xF8FFUL) | 0x05FA0000UL;
                                              // enable interrupt
  SysTick->LOAD  = 0xC34FUL;                  // LOAD freq - 1
  SysTick->VAL   = 0x0UL;                     // CURRENT = 0
  SysTick->CTRL |= 0x7UL;                     // TICKINT = 1, TICKEN = 1
}


void init_USART2(void) {
  volatile uint32_t tmp;
  RCC->AHB1ENR |= (0x1UL << 3);               // enable GPIOD clock
  RCC->APB1ENR |= (1UL << 17);                // enable USART2 clock
  tmp = RCC->AHB1ENR;                         // dummy read
  
  // fizicki dodjelit zice rx i tx na alternativnu funkciju
  // zelimo usart2 pridjelit na prikljucke 5 i 6
  GPIOD->AFR[0] |=  (7UL << 20);              // USART TX
  GPIOD->AFR[0] |=  (7UL << 24);              // USART RX
  GPIOD->MODER  &= ~(3UL << 10);              // clear bits 10 & 11
  GPIOD->MODER  |=  (2UL << 10);              // alternative function
  
  USART2->CR1  = 0x0UL;
  USART2->CR2  = 0x0UL;
  USART2->CR3  = 0x0UL;
  
  USART2->CR1 |= (1UL << 13);                 // USART enable 
  USART2->CR1 |= (0UL << 12);                 // M = 8 bits
  USART2->CR1 |= (0UL << 10);                 // PCE no pariti
  USART2->CR2 |= (0UL << 12);                 // 1 stop bit
  // formula u skripti -> 16 * freq (25M) / (8 * 2 * baud)
  USART2->BRR  = 0x0A2CUL;                    // BR => 9600 bit/s
  USART2->CR1 |= (1UL << 3);                 // TX enable 
}






