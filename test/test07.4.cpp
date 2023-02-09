#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cassert>
#include "vm_app.h"

// stress test on swap block exhaustion

int main()
{
    if (fork())
    {
        vm_yield();
        char *p = (char *)vm_map(nullptr, 0);
        std::cout << (p == nullptr) << std::endl;
        vm_yield();
        p = (char *)vm_map(nullptr, 0);
        std::cout << (p == nullptr) << std::endl;
        p[0] = 'A';
        p[1] = 'B';
        std::cout << p << std::endl;
    }
    else
    {
        for (unsigned int i = 0; i < 256; i++)
        {
            char *p = (char *)vm_map(nullptr, 0);
            p[0] = 'B';
            p[1] = 'A';
            std::cout << p << std::endl;
        }
        char *p = (char *)vm_map(nullptr, 0);
        std::cout << (p == nullptr) << std::endl;
        vm_yield();
        exit(0);
    }
    return 0;
}
