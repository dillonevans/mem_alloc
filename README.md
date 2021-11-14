mem_alloc is an optimistic dynamic memory allocator for Linux inspired by malloc. 

The initial allocation is a full page (4096 bytes most often) and if need be the allocator will request more using the sbrk system call. If additional memory cannot be allocated then NULL is returned.

Blocks are aligned by a multiple of 8 bytes for optimization purposes. For instance, if you call mem_alloc(14), it will allocate 16 bytes + an additional 16 bytes for storing the block information on the heap.

Contiguous free blocks in memory are coalesced into larger blocks to increase memory utilization. So if there six contiguous 8 byte blocks, they will be coalesced into one 64 byte block.