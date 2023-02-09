#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

int main()
{
    pid_t child_pid = fork();
    if (child_pid == 0)
    {
        char *page = (char *)vm_map(nullptr, 0);
        strcpy(page, "Hello, World");
        std::cout << page << std::endl;
    }
    else
    {
        char *page[3] = {nullptr};
        for (unsigned int i = 0; i < 3; i++)
        {
            page[i] = (char *)vm_map(nullptr, 0);
            page[i][0] = 'a';
        }
        std::cout << page[2] << std::endl;
    }
    return 0;
}

// pager: src/Pager.cpp:32: int Pager::evict(unsigned int): Assertion `result == VM_SUCCESS' failed.
