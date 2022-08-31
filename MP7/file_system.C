/*
     File        : file_system.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File System class.
                   Has support for numerical file identifiers.
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
#include "file_system.H"

/*--------------------------------------------------------------------------*/
/* CLASS Inode */
/*--------------------------------------------------------------------------*/

/* You may need to add a few functions, for example to help read and store 
   inodes from and to disk. */

/*--------------------------------------------------------------------------*/
/* CLASS FileSystem */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

FileSystem::FileSystem() {
    Console::puts("In file system constructor.\n");
    // assert(false);
}

FileSystem::~FileSystem() {
    Console::puts("unmounting file system\n");
    /* Make sure that the inode list and the free list are saved. */
    // assert(false);
    delete free_blocks;
    delete inodes;
}


/*--------------------------------------------------------------------------*/
/* FILE SYSTEM FUNCTIONS */
/*--------------------------------------------------------------------------*/


bool FileSystem::Mount(SimpleDisk * _disk) {
    Console::puts("mounting file system from disk\n");

    /* Here you read the inode list and the free list into memory */
    
    // assert(false);
    disk = _disk;
    size = disk->size();
    total_block = (int)size / 512;
    free_blocks = new int[total_block];
    memset(free_blocks, 0, sizeof(free_blocks));
    w = false;
    inodes = new Inode(-1);
    return true;
}

bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size) { // static!
    Console::puts("formatting disk\n");
    /* Here you populate the disk with an initialized (probably empty) inode list
       and a free list. Make sure that blocks used for the inodes and for the free list
       are marked as used, otherwise they may get overwritten. */
    // assert(false);
    SimpleDisk* temp = _disk;
    _disk = new SimpleDisk(DISK_ID::MASTER, _size);
    delete temp;
    return true;
}

Inode * FileSystem::LookupFile(int _file_id) {
    Console::puts("looking up file with id = "); Console::puti(_file_id); Console::puts("\n");
    /* Here you go through the inode list to find the file. */
    // assert(false);
    Inode* cur = inodes->next;
    while (cur != NULL) {
        // Console::puts("fs_93 = "); Console::puti(cur->id); Console::puts("\n");
        if (cur->id == _file_id) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

bool FileSystem::CreateFile(int _file_id) {
    Console::puts("creating file with id:"); Console::puti(_file_id); Console::puts("\n");
    /* Here you check if the file exists already. If so, throw an error.
       Then get yourself a free inode and initialize all the data needed for the
       new file. After this function there will be a new file on disk. */
    // assert(false);
    Inode* cur = inodes;
    while (cur->next != NULL){
        cur = cur->next;
    }
    cur->next = new Inode(_file_id);
    cur = cur->next;
    cur->file = new File(this, _file_id);
    return true;
}

bool FileSystem::DeleteFile(int _file_id) {
    Console::puts("deleting file with id:"); Console::puti(_file_id); Console::puts("\n");
    /* First, check if the file exists. If not, throw an error. 
       Then free all blocks that belong to the file and delete/invalidate 
       (depending on your implementation of the inode list) the inode. */
    Inode* cur = inodes->next;
    Inode* pre = inodes;
    while (cur != NULL && cur->id != _file_id) {
        cur = cur->next;
        pre = pre->next;
    }
    if (cur == NULL) {
        return true;
    }
    File* f = cur->file;
    Block* current_block = f->head->next;
    while (current_block != NULL){
        free_blocks[current_block->b_no] = 0;
        current_block = current_block->next;
    }
    pre->next = cur->next;
    delete cur;
    delete f;
    return true;
}

void FileSystem::erase(int _id) {
    // simple for loop here
    for (int i = 0; i < total_block; i++) {
        if (free_blocks[i] == _id) {
            free_blocks[i] = 0;
        }
    }
}

unsigned long FileSystem::assignBlock(int _id) {
    for (int i = 0; i < total_block; i++) {
        if (free_blocks[i] == 0) {
            free_blocks[i] = _id;
            return (unsigned long) i;
        }
    }
}