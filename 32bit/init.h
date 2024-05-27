#include <stm32f407xx.h>

void init_DefaultClock(void);
void init_Clock(void);
void init_FlashAccess(void); 
void init_SysTick(void);
void SysTick_Handler(void);
void init_USART2(void);

extern volatile uint32_t msTicks;
