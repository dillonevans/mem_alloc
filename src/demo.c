#include "../include/mem_alloc.h"

int main()
{
    byte* a = mem_alloc(256);
    byte* b = mem_alloc(512);
    mem_free(b);
    mem_free(a);
    dump_free_list();

}