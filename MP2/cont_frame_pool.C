/*
 File: ContFramePool.C
 
 Author: Jinhao Pan
 Date  : 09/20/2021
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

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
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/
// initialize static parameters in cont_frame_pool.H
ContFramePool** ContFramePool::_list;
unsigned int ContFramePool::n_pool = 0;

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no,
                             unsigned long _n_info_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // Console::puts("ContframePool::Constructor not implemented!\n");
    // assert(false);
    // bitmap fit in a single frame, 2 bits hold the state. Max number is less than or equal to 4*4K, each byte track 4 frames, follow the idea from simple_frame pool
    assert(_n_frames <= FRAME_SIZE * 4);
    // initialize all parameters
    base_frame_no = _base_frame_no;
    n_frames = _n_frames;
    info_frame_no = _info_frame_no;
    n_info_frames = _n_info_frames;
    free_frames = _n_frames;

    // info_frame_no is 0, manage info in the first frame
    if (info_frame_no == 0) {
        bitmap = (unsigned char*)(base_frame_no * FRAME_SIZE);
        free_frames--;
    }
    else {
        bitmap = (unsigned char*)(info_frame_no * FRAME_SIZE);
    }
    // list of pool should construct here
    ContFramePool::_list[ContFramePool::n_pool] = this;
    ContFramePool::n_pool++;
    assert(n_frames % 4 == 0)
    // make a rule here:
    // 00 free
    // 01 head of frame
    // inaccessible (updated, just mark all range after the frame no. as being used.)
    // 11 allocated
    //
    for (int i = 0; i < n_frames/4; i++) {
        bitmap[i] = 0x00;
    }
    // the first frame used
    if (info_frame_no == 0) {
        bitmap[0] = 0x40; //01000000
        free_frames--;
    }
    Console::puts("Frame Pool Start! Finish initialize. \n");

}

unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // Console::puts("ContframePool::get_frames not implemented!\n");
    // assert(false);
    // Console::puts("Get Frames function called, number of free frame: ");
    // Console::puti(free_frames);
    // Console::puts("\n");
    // tested successfully.
    if (free_frames < _n_frames) {
        Console::puts("There are not enough free frames. \n");
        return 0;
    }
    // implement diff, use first fit
    unsigned int diff = 0;
    while (diff <= n_frames - _n_frames) {
        // initialize useful parameters
        unsigned int byte_pos = diff / 4;
        unsigned int head_pos = diff % 4;
        unsigned int count = 0; //count frames
        unsigned int current = diff;

        // implement mask to do bitwise operation
        unsigned char mask = 0xC0 >> (2 * head_pos); // 11000000 to do right shift
        // start here
        if ((bitmap[byte_pos] & mask) == 0) {
            count = 1;
            current = diff + 1;
            while (count < _n_frames) {
                byte_pos = current / 4;
                head_pos = current % 4;
                // same mask, to check
                mask = 0xC0 >> (2 * head_pos);
                if ((bitmap[byte_pos] & mask) == 0) {
                    count++;
                    current++;
                }
                else {
                    break;
                }
            }
        }
        // try to find the head
        if (count < _n_frames) {
            diff = current + 1;
        }
        else {
            byte_pos = diff / 4;
            head_pos = diff % 4;
            mask = 0x40 >> (2 * head_pos); // 0100000 to do right shift
            bitmap[byte_pos] = bitmap[byte_pos] | mask; // implement OR operator to set head frame
            for (int i = diff + 1; i < diff + _n_frames; i++) {
                byte_pos = i / 4;
                head_pos = i % 4;
                mask = 0xC0 >> (2 * head_pos);
                // mark rest as allocated
                bitmap[byte_pos] = bitmap[byte_pos] | mask;
            }
            free_frames = free_frames - _n_frames;
            return base_frame_no + diff;
        }
    }
    return 0;
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // Console::puts("ContframePool::mark_inaccessible not implemented!\n");
    // assert(false);
    // make all frame being used
    unsigned int diff = _base_frame_no - base_frame_no;//
    unsigned int last = diff + _n_frames;
    unsigned int byte_pos = diff / 4;
    unsigned int head_pos = diff % 4;
    unsigned char mask = 0x40 >> (2 * head_pos);
    //first check
    bitmap[byte_pos] = bitmap[byte_pos] | mask;
    diff++;
    while (diff < last) {
        byte_pos = diff / 4;
        head_pos = diff % 4;
        mask = 0xC0 >> (2 * head_pos);
        bitmap[byte_pos] = bitmap[byte_pos] | mask;
        diff++;
    }
    free_frames = free_frames - _n_frames;
}

void ContFramePool::helper(unsigned long first_frame_no) {
    unsigned int diff = first_frame_no - base_frame_no;
    unsigned int byte_pos = diff / 4;
    unsigned int head_pos = diff % 4;
    unsigned char mask = 0x40 >> (2 * head_pos);
    // implement XOR operation
    bitmap[byte_pos] = bitmap[byte_pos] ^ mask; // useful for release the frame
    diff++;
    while (true) {
        byte_pos = diff / 4;
        head_pos = diff % 4;
        mask = 0xC0 >> (2* head_pos);
        // find the frame release
        if ((bitmap[byte_pos] & mask) != 0xC0) {
            break;
        }
        bitmap[byte_pos] = bitmap[byte_pos] ^ mask;
    }

}

void ContFramePool::release_frames(unsigned long _first_frame_no)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // Console::puts("ContframePool::release_frames not implemented!\n");
    // assert(false);
    // find which fram should be released
    unsigned int n = 0;
    while (_list[n]->base_frame_no + _list[n]->n_frames < _first_frame_no) {
        n++;
    }
    _list[n]->helper(_first_frame_no);
    // call helper function
    
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // Console::puts("ContframePool::need_info_frames not implemented!\n");
    // assert(false);
    /*
     Returns the number of frames needed to manage a frame pool of size _n_frames.
     The number returned here depends on the implementation of the frame pool and 
     on the frame size.
     EXAMPLE: For FRAME_SIZE = 4096 and a bitmap with a single bit per frame 
     (not appropriate for contiguous allocation) one would need one frame to manage a 
     frame pool with up to 8 * 4096 = 32k frames = 128MB of memory!
     This function would therefore return the following value:
       _n_frames / 32k + (_n_frames % 32k > 0 ? 1 : 0) (always round up!)
     Other implementations need a different number of info frames.
     The exact number is computed in this function..
     */
    // based on the intrsution
    return _n_frames * 2 / (8 * FRAME_SIZE) + (_n_frames * 2 % (8 * FRAME_SIZE) > 0 ? 1 : 0);
}
