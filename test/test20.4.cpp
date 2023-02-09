#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cassert>
#include "vm_app.h"

// file-backed page sharing

int main()
{
    if (fork() == 0)
    {
        char *file_name = (char *)vm_map(nullptr, 0);
        strcpy(file_name, "lampson83.txt");
        char *file1 = (char *)vm_map(file_name, 0);
        std::cout << file1[0] << std::endl;
        char *file2 = (char *)vm_map(file_name, 0);
        char *page1 = (char *)vm_map(nullptr, 0);
        char *page2 = (char *)vm_map(nullptr, 0);
        page1[0] = 'a';
        page2[0] = 'b';
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
