/*
     File        : file.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File class, with support for
                   sequential read/write operations.
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file.H"

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR */
/*--------------------------------------------------------------------------*/

File::File(FileSystem *_fs, int _id) {
    Console::puts("Opening file.\n");
    // assert(false);
    FS = _fs;
    id = _id;
    // if (FS->LookupFile(id) != NULL && FS->w == true) {
    //     Console::puts("L35: start here \n");
    //     size = FS->inodes->file->size;
    //     Console::puts("L36:");Console::puti(size);Console::puts("\n");
    //     position = FS->inodes->file->position;
    //     Console::puts("L37:");Console::puti(position);Console::puts("\n");
    // }
    // else {
    //     size = 0;
    //     position = 0;
    // }
    size = 0;
    position = 0;
    head = new Block(-1);
}

File::~File() {
    Console::puts("Closing file.\n");
    /* Make sure that you write any cached data to disk. */
    /* Also make sure that the inode in the inode list is updated. */
    FS->erase(id);
    delete head;
}

/*--------------------------------------------------------------------------*/
/* FILE FUNCTIONS */
/*--------------------------------------------------------------------------*/

int File::Read(unsigned int _n, char *_buf) {
    Console::puts("reading from file\n");
    // assert(false);
	// Console::puts("L55:");Console::puti(position);Console::puts("\n");
	// Console::puts("L56:");Console::puti(size);Console::puts("\n");
    if (EoF()) {
        Console::puts("EOF\n");
        return 0;
    }
    unsigned int begin = position;
    unsigned char* buffer = new unsigned char[512];
    // read file until nothing to read
    while (position < size && position - begin < _n) {
        int block_num = position / 512;
        int offset = position - block_num * 512;
        Block* cur = head->next;
        for (int i = 0; i < block_num; i++) {
            cur = cur->next;
        }
        FS->disk->read(cur->b_no, buffer);
        int j = 0;
        while (position < size && position - begin < _n && j < 512) {
            // _buf[position - begin] = buffer[j];
            // position++;
            // j++;
            _buf[position++ - begin] = buffer[j++];
        }
    }
    return position - begin;
}

int File::Write(unsigned int _n, const char *_buf) {
    Console::puts("writing to file\n");
    // assert(false);
    unsigned int begin = position;
    unsigned char* buffer = new unsigned char[512];
    while (position < size && position - begin < _n) {
        int block_num = position / 512;
        int offset = position - block_num * 512;
        Block* cur = head->next;
        for (int i = 0; i < block_num; i++) {
            cur = cur->next;
        }
        FS->disk->read(cur->b_no, buffer);
        while (position < size && position - begin < _n && position / 512 == block_num) {
            buffer[position - block_num * 512] = _buf[position - begin];
            position++;
        }
    }
    if (position - begin == _n) {
        // write all things, do not need to continue;
        return _n;
    }
    // write here
    while (position - begin < _n) {
        // new block number
        unsigned long new_block_num = FS->assignBlock(id);
        Block* cur = head;
        while (cur->next != NULL) {
            cur= cur->next;
        }
        cur->next = new Block(new_block_num);
        cur->next->next = NULL;
        int j = 0;
        while (position - begin < _n && j < 512) {
            buffer[j++] = _buf[position++ - begin];
        }
        size += j;
		// Console::puts("L118:");
		// Console::puti(size);
		// Console::puts("\n");

        FS->disk->write(new_block_num, buffer);
        FS->w = true;
    }
	// Console::puts("L124:");
	// Console::puti( position - begin);
	// Console::puts("\n");
    return position - begin;
}

void File::Reset() {
    Console::puts("resetting file\n");
    // assert(false);
    position = 0;
}

bool File::EoF() {
    Console::puts("checking for EoF\n");
    // assert(false);
    // Console::puti(position);
    // Console::puts("\n");
    // Console::puti(size);
    // Console::puts("\n");
    return position == size;
}
