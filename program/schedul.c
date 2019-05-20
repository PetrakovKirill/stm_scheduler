#include <stdint.h>
#include "stm32f10x.h"		// Это обычный CMSIS для stm32F103C8T6
#include "schedul.h"

#define F_CPU 	8000000UL

#define _ESTACK	0x20005000

#define SAVE_CONTEXT    __asm__ ( "PUSH { r4-r11 }" )
#define Save_Context(_psp)		__asm__ ( "stmia %0!,{r4-r11}" : :"r"(_psp) )

#define Load_Context(_psp)	__asm__ ("ldmia %0!,{r4-r11}" : :"r"(_psp) )

#define LOAD_CONTEXT    __asm__ ( "POP { r4-r11 }" )



// #define Get_Stack(_gs)     __asm__( "mrs r3,psp; \
// 									 mov %0, r3" : "=r"(_gs) : )

#define Get_Stack(_gs)     __asm__( "mrs %0, psp" : "=r"(_gs) : )

#define Set_Stack(_sp)     __asm__( "msr psp, %0" : : "r"(_sp) )
									 

// #define Set_Stack(_sp)     __asm__( "mov r3, %0; \
// 									 msr psp, r3" : : "r"(_sp) )

#define Set_Stack_(__sp)   __asm__( "mov sp, %0" : : "r"(__sp) )

#define Get_Stack_(__gs)   __asm__( "mov %0, sp" : "=r"(__gs) : )


// static inline __attribute__((always_inline)) void Set_Stack(unsigned int _sp) {
	// __asm__ ( "mov sp, %0" : : "r"(_sp) );
// }

//  static inline unsigned int __attribute__((always_inline)) Get_Stack(void) {
// 	unsigned int result;
// 	__asm__ ( "mov %0, sp" : "=r"(result) : );
// 	return result;
// } 

static attr_sched_t *sched;
// extern uint32_t _estack;

typedef struct _split_word {
	uint8_t low_byte;
	uint8_t high_byte;	
} split_byte_t;

typedef struct _handler_context {
	uint32_t regs[5];
	uint32_t return_addr;
	uint32_t _psr;
	uint32_t _lr;
} hdlr_cntxt_t;

#define MAIN_FUNC_STACK			0x100
#define MIN_THREAD_SIZE_STACK	0x100

void Sched_Init(attr_sched_t *schd, attr_thread_t* thrd) {
	sched = schd;
	sched->current_thread = thrd;
	sched->num_current_thread = 0;
	
	for(uint8_t i = 0; i < sched->num_threads; ++i) {
		
		/* Для нулевого потока усразу устанавливается статус RUN, так как он первый в очереди на исполнене,
		 * всем остальным потокам присваивается статус RUN_FIRST, для инициализации старта */
		if ( i ) {
			sched->current_thread[i].stack_pointer = sched->current_thread[i - 1].stack_pointer
				- sched->current_thread[i - 1].size_stack;
			sched->current_thread[i].status = FIRST_RUN;
		} else {
			// Get_Stack_(sched->current_thread[i].stack_pointer);
			sched->current_thread[i].stack_pointer = _ESTACK - MAIN_FUNC_STACK - sizeof(hdlr_cntxt_t);
			sched->current_thread[i].status = RUN;
		}
		
		/* запись в стек каждого потока адрес фунцкции исполняемого потока,
		 * для старта */
		((hdlr_cntxt_t *)sched->current_thread[i].stack_pointer)->return_addr = 
			(unsigned)sched->current_thread[i].Thread_Function;
		// sched->current_thread[i].stack_pointer -= 4;
			
		/* Оказывается адрес возврата из прерывания записывается наоборот,*/
	 	// Swap_Bytes((unsigned *)sched->current_thread[i].stack_pointer);
		
		/* Для сохранения контеста требуется 35 байт, необходимо провверить размер стека который указал пользователь,
		 * если меньше минимально установленного, тогда установить минимальный */
		if (sched->current_thread[i].size_stack < MIN_THREAD_SIZE_STACK) {
			sched->current_thread[i].size_stack = MIN_THREAD_SIZE_STACK;
		}
	}
	
	/* Настройка таймера диспетчера потоков */
	/* Инициализация SysTick */
	// Инициализация системного таймера
	SysTick->CTRL = (1<<2)| //Тактирование от системной частотыc
					(1<<1); // Разрешение прерывания
	SysTick->LOAD = ( F_CPU / 1000 ) * sched->time_thread;
	// Обнуление счетчика
	SysTick->VAL = SysTick->LOAD;
	// Старт счетчика
	SysTick->CTRL |= (1<<0);
		
	
	Set_Stack(sched->current_thread[0].stack_pointer);
	uint32_t ctrl = (1<<1);
	__asm__ ("msr control, %0" : :"r"(ctrl) );
	sched->current_thread[0].Thread_Function();
}


void SysTick_Handler( void ) {
    SCB->SHCSR |= SCB_SHCSR_SVCALLPENDED;
} 


void SVC_Handler ( void ) {
	/* Получаем адрес стека текущей задачи */
	uint32_t psp;
	Get_Stack(psp);

        switch (sched->current_thread[sched->num_current_thread].status) {
			case RUN:
				/* Сохраняем адрес возврата и устанавливаем статус */
				sched->current_thread[sched->num_current_thread].stack_pointer = psp;
				sched->current_thread[sched->num_current_thread].status = READY;
				/* сохранить контекст */
				/* Нужно отсутпить 8 слов (32 байта), что бы сохранить оставшиеся регистры (r4-r11) */
				psp -= 32;
				Save_Context(psp);
			break;
			
			case READY:
			break;
			
			case BLOCK:
			break;
			
			case WAIT_MUTEX_CAP:
			break;
			
			case FIRST_RUN: break;
		}
		
		if (++sched->num_current_thread >= sched->num_threads) {
			sched->num_current_thread = 0;
		}
		
		switch (sched->current_thread[sched->num_current_thread].status) {
			case FIRST_RUN:
				sched->current_thread[sched->num_current_thread].status = RUN;
				psp = sched->current_thread[sched->num_current_thread].stack_pointer - 4;
				Set_Stack(psp);
				// asm("reti");
                return;
				// goto without_load;
			break;
			
			case READY:
				Set_Stack(sched->current_thread[sched->num_current_thread].stack_pointer);
				sched->current_thread[sched->num_current_thread].status = RUN;
				psp = sched->current_thread[sched->num_current_thread].stack_pointer - 32;
				Load_Context(psp);
			break;
			
			case BLOCK:
			break;
			
			case WAIT_MUTEX_CAP:
			break;
			
			case RUN: break;
		}

    // LOAD_CONTEXT;
// without_load:
    return; 
}