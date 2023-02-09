#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cassert>
#include "vm_app.h"

// file-backed page sharing
// Segfault 3/20

int main()
{
    if (fork() == 0)
    {
        char *file_name = (char *)vm_map(nullptr, 0);
        strcpy(file_name, "lampson83.txt");
        char *file1 = (char *)vm_map(file_name, 0);
        std::cout << file1[0] << std::endl;
        char *file2 = (char *)vm_map(file_name, 0);
        exit(0);
    }
    else
    {
        vm_yield();
        char *file_name = (char *)vm_map(nullptr, 0);
        strcpy(file_name, "lampson83.txt");
        char *file1 = (char *)vm_map(file_name, 0);
        std::cout << file1[0] << std::endl;
    }
}
