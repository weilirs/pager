#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cassert>
#include "vm_app.h"

// Test invalid file string and mem-vim editor behavior

int main(int argc, char *argv[])
{
    char *file = (char *)vm_map("lampson83.txt", 0);
    assert(file == nullptr);
    char *swap_pages[4];
    for (unsigned int i = 0; i < 4; i++)
    {
        swap_pages[i] = (char *)vm_map(nullptr, i);
    }
    char *ptr = swap_pages[0] + VM_PAGESIZE - 4;
    strcpy(ptr, "data1.bin");
    ptr = swap_pages[1] + VM_PAGESIZE - 4;
    strcpy(ptr, "data2.bin");
    ptr = swap_pages[2] + VM_PAGESIZE - 4;
    strcpy(ptr, "data3.bin");
    if (fork())
    {
        ptr = swap_pages[0] + VM_PAGESIZE - 4;
        vm_yield();
        std::cout << ptr << std::endl;
        ptr = swap_pages[1] + VM_PAGESIZE - 4;
        file = (char *)vm_map(ptr, 1);
        strcpy(file, ptr);
        vm_yield();
        ptr = swap_pages[2] + VM_PAGESIZE - 4;
        file = (char *)vm_map(ptr, 0);
        strcpy(file, ptr);
        strcpy(ptr, "lampson83.txt");
        file = (char *)vm_map(ptr, 2);
        std::cout << file[0] << std::endl;
    }
    else
    {
        ptr = swap_pages[0] + VM_PAGESIZE - 4;
        strcpy(ptr, "lampson83.txt");
        file = (char *)vm_map(ptr, 0);
        std::cout << file[0] << std::endl;
        vm_yield();
        exit(0);
    }

    return 0;
}