#include "../include/mem_alloc.h"

int main()
{
    int* a = mem_calloc(10, sizeof(int));
    int *b = mem_calloc(100, sizeof(int));
    mem_free(a);
    mem_free(b);
    dump_free_list();
}