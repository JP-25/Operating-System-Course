#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;


void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
	// assert(false);
	kernel_mem_pool = _kernel_mem_pool;
	process_mem_pool = _process_mem_pool;
	shared_size = _shared_size;
    Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
	// assert(false);
	// kernel mode, read/write, present -> 3
	// kernel mode, read/write, not present -> 2
	// bit 0 - page present or not
	// bit 1 - read/write permission
	// bit 2 - kernel mode or user mode

	// page directory should initialize first, data for current page table
	page_directory = (unsigned long*) (process_mem_pool->get_frames(1) * PAGE_SIZE);
	// page table here
	unsigned long* new_page_table = (unsigned long*) (process_mem_pool->get_frames(1) * PAGE_SIZE);
	// kernel mode, read/write, present;
	unsigned long addr = 0;
	for (unsigned int i = 0; i < ENTRIES_PER_PAGE; i++) {
		new_page_table[i] = addr | 3; // 0011, set bit 0 and bit 1 here
		addr = addr + PAGE_SIZE;
	}
	page_directory[0] = (unsigned long) new_page_table;
	page_directory[0] = page_directory[0] | 3; // mark shared portion memory here
	page_directory[ENTRIES_PER_PAGE - 1] = (unsigned long) page_directory | 3; // last page directory entry to page directory itself
	// set rest of the entry, 1022 to invalid
	for (unsigned int i = 1; i < ENTRIES_PER_PAGE - 1; i++) {
		page_directory[i] = 0 | 2; //set bit 1 here
	}
	
	count = 0;
	// size is 32 here, set all virtual memory to null
    for (unsigned int i = 0; i < 32; i++) {
        _vm_pool_[i] = NULL;
    }
	
	Console::puts("Constructed Page Table object\n");
}


void PageTable::load()
{
	// assert(false);
	current_page_table = this;
	// write into cr3
	write_cr3((unsigned long) this->page_directory);
	Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
    // assert(false);
	paging_enabled = 1;
	// enable paging
	// reference here: http://www.osdever.net/tutorials/view/implementing-basic-paging
	write_cr0(read_cr0() | 0x80000000); //total 32bit address; first four bit marked
	Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
    // assert(false);
	unsigned long error = _r->err_code;
	if ((error &1) == 1) {
		Console::puts("error handl page fault here!");
		return;
	}
	// marked the format here, 10bit address for page table number, 10bit address for page number, and 12bit address for offset;
	// get current page directory, need to do some slightly change
	// unsigned long* current = (unsigned long*) read_cr3();
	unsigned long* current = (unsigned long*) 0xFFFFF000;
	unsigned long page_fault = read_cr2();
	// get first 10 bit page table number address;
	unsigned long page_table_no = (page_fault >> 22) & 0x3FF; // 10bit AND here, PDE here
	// get second 10 bit address for page number;
	unsigned long page_no = (page_fault >> 12) & 0x3FF; // 10 bit AND here again, PTE here
	unsigned long* page_table;
	if ((current[page_table_no] & 1) == 1) {
		// this shows that page fault already in the memory, do not hit page fault, just put it into page table
		// page_table = (unsigned long*) (current[page_table_no] & 0xFFFFF000);
		page_table = (unsigned long*)  ((page_table_no * PAGE_SIZE) | 0xFFC00000);
	}
	else if ((current[page_table_no] & 1) == 0) {
		// page fault occur, allocate memory
		current[page_table_no] = (process_mem_pool->get_frames(1) * PAGE_SIZE) | 3;
		// page table here
		page_table = (unsigned long*) ((page_table_no * PAGE_SIZE) | 0xFFC00000);
		for (unsigned int i = 0; i < ENTRIES_PER_PAGE; i++) {
			page_table[i] = 0 | 2;
		}
	}
	/* else {
		// page fault occur, allocate memory
		current[page_table_no] = (kernel_mem_pool->get_frames(1) * PAGE_SIZE) | 3;
		page_table = (unsigned long*) (current[page_table_no] & 0xFFFFF000);
		// did the same operation like the previous one again, mark the entry of page table
		for (unsigned int i = 0; i < ENTRIES_PER_PAGE; i++) {
			page_table[i] = 0 | 2;
		}
	} */
	// page the page table
	// kernel mode, read/write, present here
	page_table[page_no] = (process_mem_pool->get_frames(1) * PAGE_SIZE) | 3;
	Console::puts("handled page fault\n");
}

void PageTable::register_pool(VMPool * _vm_pool)
{
    // assert(false);
	Console::puts("registered VM pool begin. \n");
	// register the pool, size 32
    if (count < 32) {
        _vm_pool_[count++] = _vm_pool;
    } 
	else {
        Console::puts("Cannot register more pool!");
    }
}

void PageTable::free_page(unsigned long _page_no) {
    // assert(false);
	// page table number address, 10 bit, PDE here
    unsigned long page_table_no = (_page_no >> 22) & 0x3FF;
    // page table here
    unsigned long* page_table = (unsigned long*) ((page_table_no * PAGE_SIZE) | 0xFFC00000); // 11111111110000000000000000000000
    // page number, 10 bit, PTE here
    unsigned long page_no = (_page_no >> 12) & 0x3FF;
    // Release the frame
    process_mem_pool->release_frames(page_table[page_no]);
    // not present (invalid) mark
    page_table[page_no] = 0 | 2;
	// load here
    write_cr3(read_cr3());
    Console::puts("freed page\n");
}
