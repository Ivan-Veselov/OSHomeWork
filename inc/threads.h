#ifndef __THREADS_H__
#define __THREADS_H__

#define THREAD_STACK_SIZE 0x1000

struct thread {
  void* stack_mem_begin;
  void* stack_pointer;
};

typedef struct thread thread_t;
typedef void (*runnable_t)(void*);

void init_thread_system();
thread_t* thread_create(runnable_t function, void *arg);
void thread_switch(thread_t *other);
thread_t* thread_current();

#endif /*__THREADS_H__*/

