#include "../include/block_t.h"

static block_t* head = NULL;

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
        printf("Address: 0x%p, Size: %lu\n", current, current->size);
        current = current->next;
    }
}

block_t* split_block(block_t* block, size_t bytes)
{
    /*
     * Casting the address of block to a byte pointer is necessary because
     * otherwise the amount added is scaled by the amount of bytes in a block_t
     * struct
     */
    block_t* newBlock = (block_t*)((byte*)block + bytes + BLOCK_SIZE);

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