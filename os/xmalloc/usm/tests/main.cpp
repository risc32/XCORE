
#include <iostream>
#include <xmalloc>

int main()
{
    char i[1024*1024];
    xmalloc::allocator alloc(i);
    int *p = (int*)alloc.malloc(1024);
}
