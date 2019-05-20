#include "delay.h"

void Delay_ms(uint32_t arg)
{	
	// Инициализация системного таймера
	SysTick->CTRL = (1<<2); //Тактирование от системной частоты
	SysTick->LOAD = (CLK_CPU/1000) - 1;
	// Обнуление счетчика
	SysTick->VAL = 1;
	// Старт счетчика
	SysTick->CTRL |= (1<<0);
	for (volatile uint32_t i = 0; i < arg; ++i)
	{
		while(!(SysTick->CTRL&(1<<16))){}
	}
	// Останавливаем счетчик
	SysTick->CTRL &= ~(1<<0);
}

void Delay_us(uint32_t arg)
{	
	// Инициализация системного таймера
	SysTick->CTRL = (1<<2); //Тактирование от системной частоты
	SysTick->LOAD = (CLK_CPU/1000000) - 1;
	// Обнуление счетчика
	SysTick->VAL = 1;
	// Старт счетчика
	SysTick->CTRL |= (1<<0);
	for (volatile uint32_t i = 0; i < arg; ++i)
	{
		while(!(SysTick->CTRL&(1<<16))){}
	}
	// Останавливаем счетчик
	SysTick->CTRL &= ~(1<<0);
}