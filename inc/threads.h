#ifndef __THREADS_H__
#define __THREADS_H__

#include <stdint.h>

#define THREAD_STACK_SIZE 0x1000

#define THREAD_RUNNING 0
#define THREAD_JOINABLE 1

struct thread {
  void* stack_mem_begin;
  void* stack_pointer;
  
  int state;
  struct thread *prev;
  struct thread *next;
  
  uint32_t lock_count;
};

typedef struct thread thread_t;
typedef void (*runnable_t)(void*);

void init_thread_system();
void thread_schedule();
void thread_switch(thread_t *other);

thread_t* thread_current();
void thread_lock();
void thread_unlock();
thread_t* thread_create(runnable_t function, void *arg);
void thread_join(thread_t *other);

#endif /*__THREADS_H__*/

