#include <stm32f407xx.h>
#include "init.h"

volatile uint32_t msTicks;

// attribute -> forces saving all the registers being used
void __attribute__((interrupt)) SysTick_Handler(void) {
  msTicks++;
}
