#include <stdio.h>
typedef unsigned char byte;
typedef struct block_t block_t;

struct block_t
{
    block_t* next, * prev;
    size_t size;
};

#define MEGABYTE 1024 * 1024
#define MAX_BYTES MEGABYTE
#define BLOCK_SIZE ((sizeof(block_t) + 7 ) & (-8))
#define ALIGN(bytes) (bytes % 8 == 0 ? bytes : ((bytes + 7) & (-8)))
#define BLOCK(ptr) ((block_t*)((unsigned long)ptr - BLOCK_SIZE))
#define MEM(block) ((void*)((unsigned long)block + BLOCK_SIZE))
#define NEIGHBOR(block) ((block_t*)((unsigned long)block + BLOCK_SIZE + block->size))

static byte mallocBuffer[MAX_BYTES];
static block_t* head = NULL;

void init_mem_alloc()
{

    head = (block_t*)(mallocBuffer);
    head->size = MAX_BYTES - BLOCK_SIZE;
    head->next = NULL;
    head->prev = NULL;
}

void push_front(block_t* block)
{
    if (!head)
    {
        head = block;
        head->prev = NULL;
        head->next = NULL;
    }
    else
    {
        block->next = head;
        block->prev = NULL;
        head->prev = block;
        head = block;
    }
}

void insert_with_ordering(block_t* block)
{
    /**
     * If the head is null or the address of the block is less than that of the head,
     * simply add the block to the beginning of the list
     */
    if (!head || block < head)
    {
        push_front(block);
    }
    else
    {
        block_t* current = head, * next = NULL;
        while (current->next)
        {
            /**
             * If the address of the block is less than the next block in the list,
             * this block belongs between the current block and the next block
             */
            if (block < current->next)
            {
                next = current->next;

                // Insert node between current and next node
                current->next = block;
                next->prev = block;
                block->next = next;
                block->prev = current;
                return;
            }

            /**
             * Keep iterating until the block is placed or the tail is reached
             */
            current = current->next;
        }

        /**
         * Reached tail of the list. At this point, current is the tail of the list.
         * Add the block to the end of the tail.
         */
        current->next = block;
        block->next = NULL;
        block->prev = current;
    }
}

void remove_from_list(block_t* block)
{
    block_t* prev = block->prev, * next = block->next;

    if (block == head)
    {
        head = next;
        if (next) { next->prev = head; }
    }
    else
    {
        prev->next = next;
        if (next) { next->prev = prev; }
    }
}

void dump_free_list()
{
    block_t* current = head;
    printf("Contents of Free List:\n");
    while (current)
    {
        printf("Address: 0x%p, Size: %zu\n", current, current->size);
        current = current->next;
    }
}

block_t* split_block(block_t* block, size_t bytes)
{
    /*
     * Casting the address of block to an unsigned long is necessary because
     * otherwise the amount added is scaled by the amount of bytes in a block_t
     * struct
     */
    block_t* newBlock = (block_t*)((unsigned long)block + bytes + BLOCK_SIZE);

    newBlock->size = block->size - bytes - BLOCK_SIZE;
    block->size = bytes;
    return newBlock;
}

block_t* find_first_fit(size_t bytes)
{
    block_t* current = head;

    // Iterate over all blocks in the list to find the first one large enough
    while (current)
    {
        if (current->size >= bytes + BLOCK_SIZE)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void* mem_alloc(size_t bytes)
{
    // Handling spurious allocation requests
    if (bytes <= 0) { return NULL; }

    // If the free list hasn't been created, do so
    if (!head) { init_mem_alloc(); }

    // Align the data to be a multiple of 8 bytes
    size_t aligned = ALIGN(bytes);

    // Find the first block that has enough bytes available
    block_t* allocatedBlock = find_first_fit(bytes);

    // Remove the allocated block from the free list
    remove_from_list(allocatedBlock);

    // If the allocated block is a perfect fit, simply return the memory
    if (allocatedBlock->size == aligned + BLOCK_SIZE)
    {
        return MEM(allocatedBlock);
    }
    else
    {
        // Split the allocated block into two halves, with this being the remaining free half
        block_t* freeBlock = split_block(allocatedBlock, aligned);

        // Add the block to the list of free blocks
        insert_with_ordering(freeBlock);

        // Return the memory address of the allocated block's available section
        return MEM(allocatedBlock);
    }
}

void mem_copy(void* src, void* destination, size_t bytes)
{
    byte* a = (byte*)src;
    byte* b = (byte*)destination;
    for (int i = 0; i < bytes; i++)
    {
        *(b++) = *(a++);
    }
}

void merge(block_t* block, block_t* neighbor)
{
    /**
     * Consume the size of the neighboring block and remove it from the list
     */
    block->size = block->size + neighbor->size;
    remove_from_list(neighbor);
}

void coalesce_free_blocks()
{
    block_t* current = head;
    // Traverse the free-list
    while (current)
    {
        // If the neighboring block in memory is the next free block 
        if (NEIGHBOR(current) == current->next)
        {
            // Merge the current block and the next block into one
            merge(current, current->next);
        }
        current = current->next;
    }
}

void mem_free(void* ptr)
{
    // Find the block containing the memory segment pointed to
    block_t* header = BLOCK(ptr);

    // Add the block to the list of free blocks
    insert_with_ordering(header);

    // Coalesce any contiguous free-blocks
    coalesce_free_blocks();
}

int main()
{
    for (int i = 1; i < 10; i++)
    {
        int* temp = mem_alloc(i * 8);
        mem_free(temp);
    }
    dump_free_list();
}