#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout, std::endl;

int main()
{
    char *swap_pages[4];
    for (unsigned int i = 0; i < 4; i++)
    {
        swap_pages[i] = (char *)vm_map(nullptr, i);
    }
    char *ptr = swap_pages[0] + VM_PAGESIZE - 4;
    strcpy(ptr, "lampson83.txt");
    ptr = swap_pages[1] + VM_PAGESIZE - 4;
    strcpy(ptr, "data2.bin");
    ptr = swap_pages[2] + VM_PAGESIZE - 4;
    strcpy(ptr, "data3.bin");
    ptr = swap_pages[0] + VM_PAGESIZE - 4;
    char *file = (char *)vm_map(ptr, 0);
    for (unsigned int i = 0; i < 1937; i++)
    {
        std::cout << file[i];
    }
    std::cout << std::endl;
    return 0;
}
