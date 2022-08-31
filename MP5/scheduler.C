/*
 File: scheduler.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "mem_pool.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/
// extern MemPool* MEMORY_POOL;
Node::Node() {
  thread = NULL;
  next = NULL;
  pre = NULL;
}

Node::Node(Thread* _thread) {
  thread = _thread;
  next = NULL;
  pre = NULL;
}

Node* Scheduler::head = NULL;
Node* Scheduler::tail = NULL;
// system_component* sys;


/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() {
  // assert(false);
  head = new Node();
  tail = new Node();
  head->next = tail;
  tail->pre = head;
  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
  // assert(false);
  if (Machine::interrupts_enabled()) {
    Machine::disable_interrupts();
  }
  Node* cur = head->next;
  // point to the second node in ready queue
  head->next = cur->next;

  cur->next = NULL;
  cur->pre = NULL;
  Thread::dispatch_to(cur->thread);

}

void Scheduler::resume(Thread * _thread) {
  // assert(false);
  // if (!Machine::interrupts_enabled()) {
  //   Machine::enable_interrupts();
  // }
  add(_thread);
}

void Scheduler::add(Thread * _thread) {
  // assert(false);
  if (!Machine::interrupts_enabled()) {
    Machine::enable_interrupts();
  }
  Node* new_node = new Node(_thread);
  Node* temp_node = tail->pre;
  temp_node->next = new_node;
  new_node->pre = temp_node;
  new_node->next = tail;
  tail->pre = new_node;
}

void Scheduler::terminate(Thread * _thread) {
  // assert(false);
  if (head->next == tail) {
    // no thread to terminate
    return;
  }
  Node* temp_node = head->next;
  while (temp_node != tail) {
    if (temp_node->thread->ThreadId() == _thread->ThreadId()) {
      temp_node->pre->next = temp_node->next;
      temp_node->next->pre = temp_node->pre;
      // delete temp_node, and free memory
      // MEMORY_POOL->release((unsigned long)temp_node);
      delete temp_node;
      break;
    }
    temp_node = temp_node->next;
  }
}
