/*
 File: vm_pool.C
 
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

#include "vm_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) {
    // assert(false);
	// initialization here
	base_addr = _base_address;
	size = _size;
	frame_pool = _frame_pool;
	page_table = _page_table;
	// first page as the region descriptors
	rd = (region*) base_addr;
	last_addr = PageTable::PAGE_SIZE + base_addr;
	r_count = 0;
	total_size = 0;
	// page table register 
	page_table->register_pool(this);
    Console::puts("Constructed VMPool object.\n");
}

unsigned long VMPool::allocate(unsigned long _size) {
    // assert(false);
	// first check if it is out of boundary
	if (total_size + _size > size - PageTable::PAGE_SIZE || r_count == limit) {
		Console::puts("Allocate Failed! \n");
		return 0;
	}
	rd[r_count].addr = last_addr;
	rd[r_count].length = _size;
	total_size = total_size + _size;
	r_count++;
	last_addr = last_addr + _size;
    Console::puts("Allocated region of memory.\n");
	// allocate process. duplicate one _size here
	return (last_addr - _size);
}

void VMPool::release(unsigned long _start_address) {
    // assert(false);
	unsigned long i = 0;
	for (i = 0; i < r_count; i++) {
		if (rd[i].addr = _start_address) {
			break;
		}
	}
	unsigned long temp = _start_address; // do not need to change the _start_address, use temp to do the operation as the current address
	// release 
	while (temp < (_start_address + rd[i].length)) {
		// free page table
		page_table->free_page(temp);
		temp = temp + PageTable::PAGE_SIZE;
	}
	r_count--;
	total_size = total_size - rd[i].length;
	// need to update the last_addr, and update rd as well
	if (rd[i].length + rd[i].addr == last_addr) {
		last_addr = rd[i].addr;
	}
	rd[i].length = rd[r_count].length;
	rd[i].addr = rd[r_count].addr;
	// load page table
	page_table->load();
    Console::puts("Released region of memory.\n");
}

bool VMPool::is_legitimate(unsigned long _address) {
    // assert(false);
	// for loop to check
	Console::puts("Checked whether address is part of an allocated region.\n");
	for (unsigned long i = 0; i < r_count; i++) {
		if (_address <= rd[i].length + rd[i].addr && _address >= rd[i].addr) {
			// true
			Console::puts("address is part of an allocated region.\n");
			return true;
		}
	}
	Console::puts("address is not part of an allocated region.\n");
	return false;
    
}

