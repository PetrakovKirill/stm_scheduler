#include "stm32f10x.h"

#define PLL_MUL_X6	0x100000

void SysClock( void ) {
    /* Включение внешнего кварца */
	RCC->CR |= RCC_CR_HSEON;
	while( !( RCC->CR & RCC_CR_HSERDY ) );

	/* Конфигурирование PLL на системную частоту 24 МГц*/
	RCC->CFGR |= PLL_MUL_X6 | RCC_CFGR_PLLXTPRE_HSE_Div2 | RCC_CFGR_PLLSRC_HSE;
	RCC->CR |= RCC_CR_PLLON;
	while( !( RCC->CR & RCC_CR_PLLRDY ) );

	RCC->CFGR |= RCC_CFGR_SW_PLL;

	while( ( RCC->CR & 0xC ) != RCC_CFGR_SWS_PLL );
}