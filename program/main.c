#include <stdint.h>
#include "stm32f10x.h"		// СMSIS для stm32F103C8T6
#include "schedul.h"

#define NUM_THREADS			3
#define THREAD_SIZE_STACK	0x100

void Thread0(void);
void Thread1(void);
void Thread2(void);

void main( void ) {

	RCC->APB2ENR = RCC_APB2ENR_IOPCEN;
	GPIOC->CRH = 0b11 << 20;

	/* Задание парметров планировщика */
	attr_sched_t sched = {
		.num_threads = NUM_THREADS,
		.time_thread = 10
	};
	
	/* Задание параметров для каждого потока */
	attr_thread_t threads[] = { {
		.size_stack = THREAD_SIZE_STACK,
		.Thread_Function = Thread0 
	}, {
		.size_stack = THREAD_SIZE_STACK,
		.Thread_Function = Thread1
	}, {
		.size_stack = THREAD_SIZE_STACK,
		.Thread_Function = Thread2
}};
	
	/* Запуск планировщика и нулевого потока  */
	Sched_Init(&sched, &threads);
}


/* Поток 0 */
void Thread0(void) {
	while (1) {
		for (uint32_t i = 30000; i; --i);
	}
}

/* Поток 1 */
void Thread1(void) {
	while (1) {
		for (uint32_t i = 0; i < 2000; ++i);
	}
}

/* Поток 2 */
void Thread2(void) {
	while (1) {
		for (uint32_t i = ~0; i; --i);
	}
}







