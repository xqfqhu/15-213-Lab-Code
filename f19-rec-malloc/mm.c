/*
 ******************************************************************************
 *                                   mm.c                                     *
 *           64-bit struct-based implicit free list memory allocator          *
 *                  15-213: Introduction to Computer Systems                  *
 *                                                                            *
 *  ************************************************************************  *
 *                  TODO: insert your documentation here. :)                  *
 *                                                                            *
 *  ************************************************************************  *
 *  ** ADVICE FOR STUDENTS. **                                                *
 *  Step 0: Please read the writeup!                                          *
 *  Step 1: Write your heap checker. Write. Heap. checker.                    *
 *  Step 2: Place your contracts / debugging assert statements.               *
 *  Good luck, and have fun!                                                  *
 *                                                                            *
 ******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stddef.h>
#include <unistd.h>
#include <inttypes.h>

#include "mm.h"
#include "memlib.h"

/* Do not change the following! */

#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#define memset mem_memset
#define memcpy mem_memcpy
#endif /* def DRIVER */

/* You can change anything from here onward */

/*
 * If DEBUG is defined (such as when running mdriver-dbg), these macros
 * are enabled. You can use them to print debugging output and to check
 * contracts only in debug mode.
 *
 * Only debugging macros with names beginning "dbg_" are allowed.
 * You may not define any other macros having arguments.
 */
#ifdef DEBUG
/* When DEBUG is defined, these form aliases to useful functions */
#define dbg_printf(...)     printf(__VA_ARGS__)
#define dbg_requires(expr)  assert(expr)
#define dbg_assert(expr)    assert(expr)
#define dbg_ensures(expr)   assert(expr)
#define dbg_printheap(...)  print_heap(__VA_ARGS__)
#else
/* When DEBUG is not defined, no code gets generated for these */
/* The sizeof() hack is used to avoid "unused variable" warnings */
#define dbg_printf(...)     (sizeof(__VA_ARGS__), -1)
#define dbg_requires(expr)  (sizeof(expr), 1)
#define dbg_assert(expr)    (sizeof(expr), 1)
#define dbg_ensures(expr)   (sizeof(expr), 1)
#define dbg_printheap(...)  ((void) sizeof(__VA_ARGS__))
#endif


/* Basic constants */

#define ALIGNMENT 16
#define WSIZE 8
typedef uint64_t word_t;

/* change SEG_NUM to change num of segregated list. decrease throughput but increase overhead memory usage */
#define SEG_NUM 8 // each seg list contains blocks with max(block_size) = 4, 8, 16, 32, 64, 128, 256, infinite words


/* Represents the header and payload of one block in the heap */
struct block{
    word_t header; 
    struct block * pred; 
    struct block * succ; 
    word_t payload[0]; 
};
typedef struct block block_t;
#define size_empty_block sizeof(block_t)
#define size_root (size_empty_block + WSIZE)


/* Global variables */

// Pointer to first block
static char * heap_listp = NULL;


/* Function prototypes for internal helper routines */




static void write_header(block_t *block, word_t val);
static word_t byte2wordAligned(word_t size);
static int choose_list(word_t word_aligned);
static void * mm_extend_heap(size_t size);
static int check_fit(block_t * rover, word_t block_word_aligned);
static void allocate_block(block_t * rover, word_t block_word_aligned);
static int check_alloc(word_t header);
static int split(block_t * block, word_t byte);
static int check_prev_alloc(block_t * block);
static int checker_free_list_all_free();
static void checker_free_block_in_free_list();
static void checker_contiguous();
static void checker_overlap(block_t * block);
static word_t get_blockSize(word_t header);
block_t * coallese(block_t * block);
static void write_footer(block_t * block, word_t val);
static block_t * get_block(void *payload_ptr);
static void place_free_block(block_t * block);
static void write_next_header(block_t * block, word_t prev_alloc);
static void write_next_footer(block_t * block, word_t prev_alloc);
static word_t PACK(word_t val, word_t prev_alloc, word_t alloc);



/*
 * <What does this function do?>
 * <What are the function's arguments?>
 * <What is the function's return value?>
 * <Are there any preconditions or postconditions?>
 */
bool mm_init(void)
{
    if ((heap_listp = mem_sbrk(size_root * SEG_NUM + 2 * WSIZE)) == (void *) -1) return false; 
    
    heap_listp = heap_listp + WSIZE;
    word_t * epilogue = (word_t *) (heap_listp + size_root * SEG_NUM);
    * epilogue = PACK(0,1,1);
    for (int i = 0; i < SEG_NUM; i++){
        block_t * cur_root = (block_t *) (heap_listp + size_root * i);
        write_header(cur_root, PACK(4 * WSIZE, 1, 1));
        write_footer(cur_root, PACK(4 * WSIZE, 1, 1));
        cur_root->pred = NULL;
        cur_root->succ = NULL;
    }
    return true;

}


/*
 * <What does this function do?>
 * <What are the function's arguments?>
 * <What is the function's return value?>
 * <Are there any preconditions or postconditions?>
 */
void *malloc(size_t size)
{
    //fprintf(stdout, "root 7 = %d\n",  (word_t)(((block_t *) (heap_listp + 7 * size_root))->succ));
    word_t block_word_aligned = byte2wordAligned(size + 3 * WSIZE);
    int list_offset = choose_list(block_word_aligned);
    for (int i = list_offset; i < SEG_NUM; i++){
        block_t * rover = ((block_t *) (heap_listp + i * size_root))->succ;
        //fprintf(stdout, "root size = %d\n", get_blockSize(((block_t *) (heap_listp + i * size_root))->header));
        //if (rover != NULL) fprintf(stdout, "rover size = %d\n", get_blockSize(rover->header));
        //else fprintf(stdout, "rover = %d, offset = %d\n", (int) rover, i);  
        while (rover != NULL){
            //fprintf(stdout, "rover size = %d\n", get_blockSize(rover->header)); 
            if (check_fit(rover, block_word_aligned)) {
                allocate_block(rover, block_word_aligned);    
                //fprintf(stdout, "%d byte payload / %d byte block allocated via allocate_block. invariants not checked yet.\n", size, block_word_aligned * WSIZE);           
                checker_contiguous();
                checker_free_block_in_free_list();
                checker_overlap(rover);
                return rover->payload;
            }
            rover = rover->succ;
        }
    }
    block_t * new_block;
    if((new_block = mm_extend_heap(size)) != NULL) {
        //fprintf(stdout, "%d bytes allocated / %d byte block allocated via mm_extend_heap. invariants not checked yet.\n", size, block_word_aligned * WSIZE);
        checker_contiguous();
        checker_free_block_in_free_list();
        checker_overlap(new_block);
        return (new_block->payload); 
    }
    else return NULL;
}


/*
 * <What does this function do?>
 * <What are the function's arguments?>
 * <What is the function's return value?>
 * <Are there any preconditions or postconditions?>
 */
void free(void *bp)
{
    block_t * block = get_block(bp);
    write_header(block, PACK(get_blockSize(block->header), check_prev_alloc(block), 0));
    write_footer(block, PACK(get_blockSize(block->header), check_prev_alloc(block), 0));
    write_next_header(block, 0);
    write_next_footer(block, 0);
    block = coallese(block);
    //fprintf(stdout, "start place.\n");
    place_free_block(block);
    //fprintf(stdout, "%d bytes freed. invariants not checked yet.\n", block_size);
    checker_contiguous();
    checker_free_block_in_free_list();
    //fprintf(stdout, "root 7 = %d\n",  (word_t)(((block_t *) (heap_listp + 7 * size_root))->succ));
    return;
}


/*
 * <What does this function do?>
 * <What are the function's arguments?>
 * <What is the function's return value?>
 * <Are there any preconditions or postconditions?>
 */
void *realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = get_blockSize(get_block(ptr)->header) - 3 * WSIZE;
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    //fprintf(stdout, "%d bytes reallocated.\n", size);
    return newptr;
}


/*
 * <What does this function do?>
 * <What are the function's arguments?>
 * <What is the function's return value?>
 * <Are there any preconditions or postconditions?>
 */
void *calloc(size_t elements, size_t size)
{
    void *bp;
    size_t asize = elements * size;

    if (asize/elements != size)
    {
        // Multiplication overflowed
        return NULL;
    }

    bp = malloc(asize);
    if (bp == NULL)
    {
        return NULL;
    }

    // Initialize all bits to 0
    memset(bp, 0, asize);

    return bp;
}


/******** The remaining content below are helper and debug routines ********/

/*
 * <What does this function do?>
 * <What are the function's arguments?>
 * <What is the function's return value?>
 * <Are there any preconditions or postconditions?>
 */
static word_t PACK(word_t val, word_t prev_alloc, word_t alloc){
    return ((val) | (prev_alloc<<1) |(alloc));
}

static void write_next_header(block_t * block, word_t prev_alloc){
    block_t * next_block = (block_t *) (((void *) block) + get_blockSize(block->header));
    word_t val = PACK(get_blockSize(next_block->header), prev_alloc, check_alloc(next_block->header));
    write_header(next_block, val);
}

static void write_next_footer(block_t * block, word_t prev_alloc){
    block_t * next_block = (block_t *) (((void *) block) + get_blockSize(block->header));
    if (!check_alloc(next_block->header)){
        word_t val = PACK(get_blockSize(next_block->header), prev_alloc, check_alloc(next_block->header));
        write_footer((block_t *) (((void *) block) + get_blockSize(block->header)), val);
    }
}
static void write_header(block_t *block, word_t val){
    block->header = val;
}


static void write_footer(block_t * block, word_t val){
    * ((word_t *) (((void *) block) + get_blockSize(block->header) - WSIZE)) = val;
    return;
}

static void write_epilogue(block_t * block){
    * ((word_t *) (((void *) block) + get_blockSize(block->header))) = PACK(0, 1, 1);
    return;
}


static word_t byte2wordAligned(word_t size){
    word_t word;
    word_t word_aligned;
    (size % WSIZE == 0) ? (word = size / WSIZE) : (word = size / WSIZE + 1);
    (word % 2 == 0) ? (word_aligned = word) : (word_aligned = word + 1);
    return word_aligned;
}

static int choose_list(word_t word_aligned){
    if (word_aligned <= 4) return 0;
    else if (word_aligned <= 8) return 1;
    else if (word_aligned <= 16) return 2;
    else if (word_aligned <= 32) return 3;
    else if (word_aligned <= 64) return 4;
    else if (word_aligned <= 128) return 5;
    else if (word_aligned <= 256) return 6;
    else return 7;
}

static void * mm_extend_heap(size_t size){
    word_t block_word_aligned = byte2wordAligned(size + 3 * WSIZE);
    void * new_block;
    if ((new_block = mem_sbrk(block_word_aligned * WSIZE)) == (void *) -1) return NULL;
    new_block = new_block - WSIZE;
    write_header((block_t *) new_block, PACK(block_word_aligned * WSIZE, check_prev_alloc((block_t *) new_block),1));
    write_epilogue((block_t *) new_block);
    return new_block;
}

static int check_fit(block_t * rover, word_t block_word_aligned){
    
    return (get_blockSize(rover->header) >= block_word_aligned * WSIZE);
}
static void allocate_block(block_t * rover, word_t block_word_aligned){
    if (split(rover, block_word_aligned * WSIZE)) return;
    write_header(rover, PACK(get_blockSize(rover->header), check_prev_alloc(rover) ,1));
    block_t * pred = rover->pred;
    block_t * succ = rover->succ;
    pred->succ = succ;
    if (succ != NULL) succ->pred = pred;

    block_t * next = (void *) rover + block_word_aligned * WSIZE;
    write_header(next, PACK(get_blockSize(next->header), 1, check_alloc(next->header)));
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


block_t * coallese(block_t * block){
    word_t coallesed_size = get_blockSize(block->header);
    if (!check_prev_alloc(block)){
        word_t *prev_footer = (word_t *) (((void *) block) - WSIZE);
        word_t prev_size = get_blockSize(* prev_footer);
        coallesed_size += prev_size;
        block_t * prev_block = (block_t *) (((void *) block) - prev_size);
        (prev_block->pred)->succ = prev_block->succ;
        if (prev_block->succ != NULL) (prev_block->succ)->pred = prev_block->pred;
        //fprintf(stdout, "prev_block->pred size = %d\n", get_blockSize((prev_block->pred)->header));
        //fprintf(stdout, "block->succ = %d\n", (int) (block->succ));
        //if (block->succ != NULL) fprintf(stdout, "block->succ size = %d", get_blockSize((block->succ)->header));
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
    word_t * new_next_header = (word_t *) (((void *) block) + coallesed_size);
    * new_next_header = PACK(get_blockSize(*new_next_header), 0, check_alloc(*new_next_header));
    return block;
}
void static place_free_block(block_t * block){
    word_t block_size_word = get_blockSize(block->header) / WSIZE;
    block_t * rover = (block_t *) (heap_listp + choose_list(block_size_word) * size_root);
    word_t block_addr = (word_t) block;
    block_t * pred_rover = NULL;

    while (rover != NULL){
        if (((word_t) rover) > block_addr) break;
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


/*
 * <What does this function do?>
 * <What are the function's arguments?>
 * <What is the function's return value?>
 * <Are there any preconditions or postconditions?>
 */


static int checker_free_list_all_free(){
    int num_free_list_blocks = 0;
    for (int i = 0; i < SEG_NUM; i++){
        block_t * block = ((block_t *) (heap_listp + size_root * i))->succ;
        while (block != NULL){
            if (check_alloc(block->header)) {
                fprintf(stderr, "free list contains allocated block\n");
	            exit(1);
            }
            //fprintf(stdout, "%d byte block unallocated(traverse free list %d)\n", get_blockSize(block->header), i);
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
            //fprintf(stdout, "%d byte block unallocated(traverse heap)\n", blockSize);
        }
        p += blockSize;
    }
    if (num_free_list_blocks != num_free_blocks){
        fprintf(stderr, "free list contains %d blocks while there are %d free blocks. some free blocks are not in any free list\n", num_free_list_blocks, num_free_blocks);
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
	        exit(1);
        }
        p += blockSize;

    }
}



bool mm_checkheap(int line)
{
    /*
     * TODO: Delete this comment!
     *
     * You will need to write the heap checker yourself.
     * Please keep modularity in mind when you're writing the heap checker!
     *
     * As a filler: one guacamole is equal to 6.02214086 x 10**23 guacas.
     * One might even call it...  the avocado's number.
     *
     * Internal use only: If you mix guacamole on your bibimbap,
     * do you eat it with a pair of chopsticks, or with a spoon?
     */

    return true;

}


