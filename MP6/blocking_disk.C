/*
     File        : blocking_disk.c

     Author      : 
     Modified    : 

     Description : 

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "blocking_disk.H"

extern Scheduler* SYSTEM_SCHEDULER;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {
}

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/

// void BlockingDisk::wait_until_ready() {
//   if (!SimpleDisk::is_ready()) {
//     Thread* cur = Thread::CurrentThread();
//     SYSTEM_SCHEDULER->add_block(cur);
//     SYSTEM_SCHEDULER->yield();
//   }
// }

void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  // SimpleDisk::read(_block_no, _buf);
  // make issue_operation to public
  issue_operation(READ, _block_no);
  if (!SimpleDisk::is_ready()) {
    Thread* cur = Thread::CurrentThread();
    SYSTEM_SCHEDULER->add_block(cur);
    SYSTEM_SCHEDULER->yield();
  }
  // from simple_disk
  int i;
  unsigned short tmpw;
  for (i = 0; i < 256; i++) {
    tmpw = Machine::inportw(0x1F0);
    _buf[i*2]   = (unsigned char)tmpw;
    _buf[i*2+1] = (unsigned char)(tmpw >> 8);
  }
}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  // SimpleDisk::write(_block_no, _buf);
  // make issue_operation to public
  issue_operation(WRITE, _block_no);
  if (!SimpleDisk::is_ready()) {
    Thread* cur = Thread::CurrentThread();
    SYSTEM_SCHEDULER->add_block(cur);
    SYSTEM_SCHEDULER->yield();
  }
  // from simple_disk
  int i; 
  unsigned short tmpw;
  for (i = 0; i < 256; i++) {
    tmpw = _buf[2*i] | (_buf[2*i+1] << 8);
    Machine::outportw(0x1F0, tmpw);
  }
}
