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
Node* Scheduler::block_head = NULL;
Node* Scheduler::block_tail = NULL;


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
  // this is for the ready queue
  head = new Node();
  tail = new Node();
  head->next = tail;
  tail->pre = head;
  // this is for the block queue
  block_head = new Node();
  block_tail = new Node();
  block_head->next = block_tail;
  block_tail->pre = block_head;

  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
  // assert(false);
  // if (Machine::interrupts_enabled()) {
  //   Machine::disable_interrupts();
  // }
  Node* cur = head->next;
  // point to the second node in ready queue
  head->next = cur->next;

  cur->next->pre = head;
  Thread::dispatch_to(cur->thread);
  delete cur;

}

void Scheduler::resume(Thread * _thread) {
  // assert(false);
  // if (!Machine::interrupts_enabled()) {
  //   Machine::enable_interrupts();
  // }
  add(_thread);
  //check block queue
  if (block_head->next == block_tail) {
    return;
  }
  // get information from hand out, check disk
  if (!((Machine::inportb(0x1F7) & 0x08) != 0)) {
    return;
  }
  // remove first node from block queue here
  Node* temp_node = block_head->next;
  block_head->next = temp_node->next;
  temp_node->next->pre = block_head;
  add(temp_node->thread); // add again
  delete temp_node;
}

void Scheduler::add(Thread * _thread) {
  // assert(false);
  // if (!Machine::interrupts_enabled()) {
  //   Machine::enable_interrupts();
  // }
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

void Scheduler::add_block(Thread* _thread) {
  // same operation like add function
  Node* new_node = new Node(_thread);
  Node* temp_node = block_tail->pre;
  temp_node->next = new_node;
  new_node->pre = temp_node;
  new_node->next = block_tail;
  block_tail->pre = new_node;
}
