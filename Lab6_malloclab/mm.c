/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h> 
#include<stdbool.h>   

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Cecil Sagehen",
    /* First member's email address */
    "cecil.sagehen@pomona.edu",
    /* Second member's full name */
    "",
    /* Second member's email address */
    ""
};

/*
 * malloc that supports 32 bit address space
 * addresses are 8 byte aligned
 */
/* change ALIGNMENT, WSIZE, word_t to suppport 64 it address space */
#define ALIGNMENT 16
#define WSIZE 8
typedef u_int64_t word_t;


/* change SEG_NUM to change num of segregated list. decrease throughput but increase overhead memory usage */
#define SEG_NUM 12 // each seg list contains blocks with max(block_size) = 4, 8, 16, 32, 64, 128, 256, infinite words
static word_t seg_max[SEG_NUM-1];

/* heap struct */
static char * heap_listp = NULL;

struct block{
    word_t header; 
    struct block * pred; 
    struct block * succ; 
    word_t payload[0]; 
};
typedef struct block block_t;
#define size_empty_block sizeof(block_t)
#define size_root (size_empty_block + WSIZE)

static void write_header(block_t *block, word_t val);
static word_t byte2wordAligned(word_t size);
static int choose_list(word_t word_aligned);
static void * mm_extend_heap(word_t word_aligned, word_t alloc);
static void allocate_block(block_t * rover, word_t block_word_aligned);
static int check_alloc(word_t header);
static int split(block_t * block, word_t byte);
static int check_prev_alloc(block_t * block);
static int checker_free_list_all_free();
static void checker_free_block_in_free_list();
static void checker_contiguous();
static void checker_overlap(block_t * block); 
static word_t get_blockSize(word_t header);
block_t * coallesce(block_t * block);
static void write_footer(block_t * block, word_t val);
static block_t * get_block(void *payload_ptr);
static void place_free_block(block_t * block);
static void write_next_hf(block_t * block, word_t prev_alloc);
static word_t PACK(word_t val, word_t prev_alloc, word_t alloc);
static void write_epilogue(block_t * block, word_t alloc);
static void debug_stop();
static block_t * find_fit(word_t block_word_aligned);
static void check_heap_len(block_t * block, word_t size, bool alloc);

//#define PERFORMANCE

/* #ifdef DEBUG
#define DBG_PRINTF(...) fprintf(stdout, __VA_ARGS__)
#define CHECKER_FREE_BLOCK_IN_FREE_LIST() checker_free_block_in_free_list()
#define CHECKER_CONTIGUOUS() checker_contiguous()
#define CHECKER_OVERLAP(rover) checker_overlap(rover) 
#define CHECK_HEAP_LEN(block, size, alloc) */

#ifdef PERFORMANCE
#define DBG_PRINTF(...) 
#define CHECKER_FREE_BLOCK_IN_FREE_LIST() 
#define CHECKER_CONTIGUOUS() 
#define CHECKER_OVERLAP(rover)
#define CHECK_HEAP_LEN(block, size, alloc) check_heap_len(block, size, alloc) 


#else 
#define DBG_PRINTF(...) 
#define CHECKER_FREE_BLOCK_IN_FREE_LIST() 
#define CHECKER_CONTIGUOUS() 
#define CHECKER_OVERLAP(rover)
#define CHECK_HEAP_LEN(block, size, alloc)
#endif


/* 
 * mm_init - initialize the malloc package.
 */

int mm_init(void)
{
    if ((heap_listp = mem_sbrk(size_root * SEG_NUM + 2 * WSIZE)) == (void *) -1) return -1; 
    
    heap_listp = heap_listp + WSIZE;
    word_t * epilogue = (word_t *) (heap_listp + size_root * SEG_NUM);
    * epilogue = PACK(0,1,1);
    for (int i = 0; i < SEG_NUM; i++){
        block_t * cur_root = (block_t *) (heap_listp + size_root * i);
        write_header(cur_root, PACK(0, 1, 1));
        write_footer(cur_root, PACK(0, 1, 1));
        cur_root->pred = NULL;
        cur_root->succ = NULL;
    }
    word_t seg_max_i = 4;
    for(int i = 0; i < (SEG_NUM-1); i++){
        seg_max[i] = seg_max_i;
        seg_max_i = seg_max_i * 2;
    }
    return 0;

}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */



void *mm_malloc(size_t size)
{
    word_t block_word_aligned = byte2wordAligned(size + 3 * WSIZE);
    block_t * rover;
    if ((rover = find_fit(block_word_aligned)) != NULL){
        allocate_block(rover, block_word_aligned);    
        DBG_PRINTF("%lu byte payload via allocate_block. invariants not checked yet.\n", size);           
        CHECKER_CONTIGUOUS();
        CHECKER_FREE_BLOCK_IN_FREE_LIST();
        CHECKER_OVERLAP(rover);
        return rover->payload;
    }
    block_t * new_block;
    if((new_block = mm_extend_heap(block_word_aligned, 1)) != NULL) {
        word_t block_word_aligned_free;
        //word_t block_word_aligned_free = byte2wordAligned(size / 2 + 1 + 3 * WSIZE);
        //(block_word_aligned_free <= seg_max[SEG_NUM-2])? (block_word_aligned_free = block_word_aligned_free) : (block_word_aligned_free = seg_max[SEG_NUM-2]);
        block_word_aligned_free = block_word_aligned;
        block_t * new_free_block;

        if((new_free_block = mm_extend_heap(block_word_aligned_free, 0)) != NULL){
            place_free_block(new_free_block);
            DBG_PRINTF("%lu bytes allocated via mm_extend_heap. invariants not checked yet.\n", size);
            CHECKER_CONTIGUOUS();
            CHECKER_FREE_BLOCK_IN_FREE_LIST();
            CHECKER_OVERLAP(rover);
            CHECK_HEAP_LEN(new_free_block, size, true);
            return (new_block->payload); 
        }
        else return NULL;
    }
    else return NULL;
}


/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    block_t * block = get_block(ptr);
    word_t header = PACK(get_blockSize(block->header), check_prev_alloc(block), 0);
    write_header(block, header);
    write_footer(block, header);
    write_next_hf(block, 0);
    block = coallesce(block);
    place_free_block(block);
    DBG_PRINTF("%lu bytes freed. invariants not checked yet.\n", block_size);
    CHECKER_CONTIGUOUS();
    CHECKER_FREE_BLOCK_IN_FREE_LIST();
    return;
}



/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    if (ptr == NULL) return mm_malloc(size);
    if (size == 0) return NULL;

    word_t oldpayload_size = get_blockSize(get_block(ptr)->header) - 3 * WSIZE;
    word_t oldpayload_lastword = *((word_t *) (ptr + oldpayload_size - WSIZE));
    block_t * newblock;
    word_t newblock_size_word_aligned = byte2wordAligned(size + 3 * WSIZE);
    word_t copySize;
    (oldpayload_size < size)? (copySize = oldpayload_size) : (copySize = size);

    mm_free(ptr);
    if ((newblock = find_fit(newblock_size_word_aligned)) != NULL){
        void * newpayload = newblock->payload;
        memmove(newpayload, ptr, copySize);
        if ((newblock_size_word_aligned * WSIZE - 3 * WSIZE) >= oldpayload_size) memcpy(newpayload+oldpayload_size-WSIZE, &oldpayload_lastword, WSIZE);
        allocate_block(newblock, newblock_size_word_aligned);
        DBG_PRINTF("%lu bytes reallocated\n", size);
        CHECKER_CONTIGUOUS();
        CHECKER_FREE_BLOCK_IN_FREE_LIST();
        CHECKER_OVERLAP(newblock);
        return newpayload;
    }
    
    if((newblock = mm_extend_heap(newblock_size_word_aligned, 1)) != NULL) {
        void * newpayload = newblock->payload;
        memcpy(newpayload, ptr, copySize);
        if ((newblock_size_word_aligned * WSIZE - 3 * WSIZE) >= oldpayload_size) memcpy(newpayload+oldpayload_size-WSIZE, &oldpayload_lastword, WSIZE);
        CHECK_HEAP_LEN(get_block(newpayload), size, false);
        //word_t block_word_aligned_free = byte2wordAligned(size / 2 + 1 + 3 * WSIZE);
        //(block_word_aligned_free <= seg_max[SEG_NUM-2])? (block_word_aligned_free = block_word_aligned_free) : (block_word_aligned_free = seg_max[SEG_NUM-2]); 
        return (newpayload); 
    }
    else return NULL;
    

}


/* helper functions */
static word_t byte2wordAligned(word_t size){
    word_t word;
    word_t word_aligned;
    (size % WSIZE == 0) ? (word = size / WSIZE) : (word = size / WSIZE + 1);
    (word % 2 == 0) ? (word_aligned = word) : (word_aligned = word + 1);
    return word_aligned;
}

static int choose_list(word_t word_aligned){
    for (int i = 0; i < (SEG_NUM - 1); i++){
        if (word_aligned <= seg_max[i]) return i;
    }
    return (SEG_NUM-1);
}

static void * mm_extend_heap(word_t word_aligned, word_t alloc){
    void * new_block;
    if ((new_block = mem_sbrk(word_aligned * WSIZE)) == (void *) -1) return NULL;
    new_block = new_block - WSIZE;
    word_t header = PACK(word_aligned * WSIZE, check_prev_alloc((block_t *) new_block),alloc);
    write_header((block_t *) new_block, header);
    if (alloc == 0) write_footer((block_t *) new_block, header);
    write_epilogue((block_t *) new_block, alloc);
    return new_block;
}

static void allocate_block(block_t * rover, word_t block_word_aligned){
    if (split(rover, block_word_aligned * WSIZE)) return;
    write_header(rover, PACK(get_blockSize(rover->header), check_prev_alloc(rover) ,1));
    write_next_hf(rover,1);
    block_t * pred = rover->pred;
    block_t * succ = rover->succ;
    pred->succ = succ;
    if (succ != NULL) succ->pred = pred;
    return;
}
static int check_alloc(word_t header){
    return (header & 1);
}
static int split(block_t * block, word_t byte){
    word_t sizeLeft = get_blockSize(block->header) - byte;
    if (sizeLeft > (size_empty_block + WSIZE)){
        write_header(block, PACK(byte, check_prev_alloc(block) ,1));
        block_t * pred = block->pred;
        block_t * succ = block->succ;
        pred->succ = succ;
        if (succ != NULL) succ->pred = pred;
        block_t * block_left = (block_t *) (((void *) block) + byte);
        write_header(block_left, PACK(sizeLeft, 1, 0));
        write_footer(block_left, PACK(sizeLeft, 1, 0));
        place_free_block(block_left);
        return 1;
    }
    return 0;
}

static int check_prev_alloc(block_t * block){
    return (((block->header) & 2) >> 1);
}

static block_t * get_block(void *payload_ptr){
    return (block_t *) (payload_ptr - 3 * WSIZE);
}

block_t * coallesce(block_t * block){
    word_t coallesed_size = get_blockSize(block->header);
    if (!check_prev_alloc(block)){
        word_t *prev_footer = (word_t *) (((void *) block) - WSIZE);
        word_t prev_size = get_blockSize(* prev_footer);
        coallesed_size += prev_size;
        block_t * prev_block = (block_t *) (((void *) block) - prev_size);
        (prev_block->pred)->succ = prev_block->succ;
        if (prev_block->succ != NULL) (prev_block->succ)->pred = prev_block->pred;
        block = prev_block;
    }
    word_t * next_header = (word_t *) (((void *) block) + coallesed_size);
    if (!check_alloc(* next_header)){
        word_t next_size = get_blockSize(* next_header);
        coallesed_size += next_size;
        block_t * next_block = (block_t *) next_header;
        if (next_block->succ != NULL) (next_block->succ)->pred = next_block->pred;
        (next_block->pred)->succ = next_block->succ;
    }
    write_header(block, PACK(coallesed_size, check_prev_alloc(block), 0));
    write_footer(block, PACK(coallesed_size, check_prev_alloc(block), 0));
    write_next_hf(block, 0);
    return block;
}
static void place_free_block(block_t * block){
    word_t block_size_word = get_blockSize(block->header) / WSIZE;
    block_t * pred_rover = (block_t *) (heap_listp + choose_list(block_size_word) * size_root);
    block_t * rover = pred_rover->succ;
    word_t block_addr = (word_t) block;

    while (rover != NULL){
        word_t cur_addr = (word_t) rover;
        if (cur_addr > block_addr) break;
        pred_rover = rover;
        rover = rover->succ;
    }
    if (rover != NULL){
        (rover->pred)->succ = block;
        block->pred = rover->pred;
        rover->pred = block;
        block->succ = rover;
        return;
    }
    else{
        pred_rover->succ = block;
        block->pred = pred_rover;
        block->succ = NULL;
        return;
    }
}

static word_t get_blockSize(word_t header){
    return (header & (~7));
}

static word_t PACK(word_t val, word_t prev_alloc, word_t alloc){
    return ((val) | (prev_alloc<<1) |(alloc));
}

static void write_next_hf(block_t * block, word_t prev_alloc){
    block_t * next_block = (block_t *) (((void *) block) + get_blockSize(block->header));
    word_t val = PACK(get_blockSize(next_block->header), prev_alloc, check_alloc(next_block->header));
    write_header(next_block, val);
    if (!check_alloc(next_block->header)){
        word_t val = PACK(get_blockSize(next_block->header), prev_alloc, check_alloc(next_block->header));
        write_footer(next_block, val);
    }
}


static void write_header(block_t *block, word_t val){
    block->header = val;
}


static void write_footer(block_t * block, word_t val){
    * ((word_t *) (((void *) block) + get_blockSize(block->header) - WSIZE)) = val;
    return;
}

static void write_epilogue(block_t * block, word_t alloc){
    * ((word_t *) (((void *) block) + get_blockSize(block->header))) = PACK(0, alloc, 1);
    return;
}

 

static int checker_free_list_all_free(){
    int num_free_list_blocks = 0;
    for (int i = 0; i < SEG_NUM; i++){
        block_t * block = ((block_t *) (heap_listp + size_root * i))->succ;
        while (block != NULL){
            if (check_alloc(block->header)) {
                fprintf(stderr, "free list contains allocated block\n");
                debug_stop();
	            exit(1);
            }
            DBG_PRINTF("%lu byte block unallocated(traverse free list %d)\n", get_blockSize(block->header), i);
            block = block->succ;
            num_free_list_blocks += 1;
        }
    }
    return num_free_list_blocks;
}

static void checker_free_block_in_free_list(){
    int num_free_list_blocks = checker_free_list_all_free();
    int num_free_blocks = 0;
    void * p = heap_listp + SEG_NUM * size_root;
    while ((*((word_t *) p)) != PACK(0,1,1) && (*((word_t *) p)) != PACK(0,0,1)) {
        word_t blockSize = get_blockSize(((block_t *) p)->header);
        if (!check_alloc(((block_t *) p)->header)) {
            num_free_blocks++;
            DBG_PRINTF("%lu byte block unallocated(traverse heap)\n", blockSize);
        }
        p += blockSize;
    }
    if (num_free_list_blocks != num_free_blocks){
        fprintf(stderr, "free list contains %d blocks while there are %d free blocks. some free blocks are not in any free list\n", num_free_list_blocks, num_free_blocks);
	    debug_stop();
        exit(1);
    }
}

static void checker_contiguous(){
    void * p = heap_listp + SEG_NUM * size_root;
    int prev_free = 0;
    word_t prev_size = 0;
    word_t cur_size = 0;
    while ((*((word_t *) p)) != PACK(0,1,1) && (*((word_t *) p)) != PACK(0,0,1)) {
        int cur_free = (!check_alloc(((block_t *) p)->header));
        cur_size = get_blockSize(((block_t *) p)->header);
        if (prev_free && cur_free){
            fprintf(stderr, "contiguous free blocks. size 1 = %lu, size 2 = %lu\n", cur_size, prev_size);
	        debug_stop();
            exit(1);
        }
        prev_free = cur_free;
        prev_size = cur_size;
        word_t blockSize = get_blockSize(((block_t *) p)->header);
        p += blockSize;

    }
}
static void checker_overlap(block_t * block){
    word_t start = (word_t) block;
    word_t end = start + get_blockSize(block->header);

    void * p = heap_listp + SEG_NUM * size_root;
    while ((*((word_t *) p)) != PACK(0,1,1) && (*((word_t *) p)) != PACK(0,0,1)) {
        word_t cur_start = (word_t) p;
        word_t blockSize = get_blockSize(*((word_t *) p));
        word_t cur_end = cur_start + blockSize;

        if (((start < cur_start) && (cur_start < end)) || ((start == cur_start) && (cur_end != end)) || ((start < cur_end) && (cur_end < end)) || ((start != cur_start) && (cur_end == end))){
            fprintf(stderr, "overlapping blocks with size %lu and %lu\n", blockSize, get_blockSize(start));
	        debug_stop();
            exit(1);
        }
        p += blockSize;

    }
}

static void debug_stop(){
    fprintf(stdout, "stop hear to debug");
    return;
}

static block_t * find_fit(word_t block_word_aligned){
    word_t block_byte_aligned = block_word_aligned * WSIZE;
    int list_offset = choose_list(block_word_aligned);
    //int block_visited = 0;
    for (int i = list_offset; i < SEG_NUM; i++){
        block_t * rover = ((block_t *) (heap_listp + i * size_root))->succ;
        while (rover != NULL){
            //block_visited++;
            if (get_blockSize(rover->header) >= block_byte_aligned) {
                
                return rover;
            }
            rover = rover->succ;
        }
    }
    return NULL;
}
 



static void check_heap_len(block_t * block, word_t size, bool alloc){
    word_t block_size = get_blockSize(block->header);
    word_t heap_size = (word_t) block + block_size - (word_t )heap_listp;
    if (alloc) fprintf(stdout, "%lu bytes allocated: current heap size is extended to %lu\n", size, heap_size);
    else fprintf(stdout, "%lu bytes reallocated: current heap size is extended to %lu\n", size, heap_size);
    return;
}