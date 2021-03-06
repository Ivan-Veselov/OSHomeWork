#include <threads.h>

#include <malloc.h>
#include <buddy_alloc.h>
#include <int_controller.h>

#include <io.h>

struct initial_thread_frame  {
  uint64_t r15;
  uint64_t r14;
  uint64_t r13;
  uint64_t r12;
  uint64_t rbx;
  uint64_t rbp;
  
  uint64_t return_address;
} __attribute__((packed));

typedef struct initial_thread_frame initial_thread_frame_t;

extern void __thread_origin(void);
extern void switch_threads(void **old_sp, void *new_sp);

thread_t *current_thread;
thread_t kernel_thread;

void init_thread_system() {
  current_thread = &kernel_thread;
  
  current_thread->stack_mem_begin = NULL;
  current_thread->stack_pointer = NULL;
  
  current_thread->state = THREAD_RUNNING;
  current_thread->prev = current_thread;
  current_thread->next = current_thread;
  
  current_thread->lock_count = 0;
}

void thread_schedule() {
  thread_switch(current_thread->next);
}

void thread_lock() {
  if (current_thread->lock_count == 0) {
    __asm__("cli");
  }
  
  current_thread->lock_count++;
}

void thread_unlock() {
  current_thread->lock_count--;
  
  if (current_thread->lock_count == 0) {
    __asm__("sti");
  }
}

void thread_origin(runnable_t function, void *arg) {
  end_of_interrupt_master(); // First call was made from interruption handler
  __asm__("sti"); // Interruption handler switched off interruptions

	function(arg);

  thread_lock();
  current_thread->state = THREAD_JOINABLE;
  thread_unlock();
  
  while (1);
}

thread_t* thread_create(runnable_t function, void *arg) {
  thread_lock();
  
  thread_t *new_thread = (thread_t*)malloc(sizeof(thread_t));

  new_thread->stack_mem_begin = malloc(THREAD_STACK_SIZE);
  new_thread->stack_pointer =
            (void*)((uint64_t)new_thread->stack_mem_begin + THREAD_STACK_SIZE);

  new_thread->stack_pointer = (void*)((uint64_t)new_thread->stack_pointer
                                            - sizeof(initial_thread_frame_t));

  new_thread->state = THREAD_RUNNING;

  new_thread->prev = current_thread;
  new_thread->next = current_thread->next;
  
  new_thread->prev->next = new_thread;
  new_thread->next->prev = new_thread;

  initial_thread_frame_t *frame = new_thread->stack_pointer;

  frame->r15 = (uint64_t)function;
  frame->r14 = (uint64_t)arg;
  frame->r13 = 0;
  frame->r12 = 0;
  frame->rbx = 0;
  frame->rbp = 0;
  
  frame->return_address = (uint64_t)(&__thread_origin);
 
  thread_unlock(); 
  return new_thread;
}

void thread_join(thread_t *other) {
  while (1) {
    thread_lock();
    if (other->state != THREAD_JOINABLE) {
      thread_unlock();
      continue;
    }
    
    break;
  }
  
  // There are at least two threads in list
  other->prev->next = other->next;
  other->next->prev = other->prev;
  
  free(other);
  
  thread_unlock();
}

void thread_switch(thread_t *other) {
  if (other == current_thread) {
    return;
  }

  thread_t *previous = current_thread;
  current_thread = other;
  
  switch_threads(&previous->stack_pointer, other->stack_pointer);
}

thread_t* thread_current() {
  return current_thread;
}

