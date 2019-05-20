#ifndef _SCHEDUL_H
#define _SCHEDUL_H

#include <stdint.h>

/* Статусы потоков */
typedef enum {
	FIRST_RUN = 0,	/* Готов к первому запуску */
	READY,			/* готов к исполнению */
	RUN,			/* исполняется */
	BLOCK,			/* поток заблокирован */
	WAIT_MUTEX_CAP	/* поток заблокирован в ожидании захвата Mutex-а */
} thread_status_t;

typedef struct _attr_thread {
	thread_status_t status;						/* Статус потока */
	unsigned		size_stack;					/* Размер стека  */
	unsigned		stack_pointer;				/* Указатель стека */
	void			(*Thread_Function)(void);	/* Указатель на функцию потока */
} attr_thread_t;

typedef struct _attr_sched {
	uint8_t			num_threads;				/* Количество потоков */
	uint8_t			time_thread;				/* время работы потока */
	uint8_t			num_current_thread;			/* номер текущего потока */
	attr_thread_t	*current_thread;
} attr_sched_t;

void Sched_Init(attr_sched_t *schd, attr_thread_t *thrd);

#endif /* _SCHEDUL_H */