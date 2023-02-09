#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cassert>
#include "vm_app.h"

// fork with no swap block

using std::cout, std::endl;

int main()
{
    for (unsigned int i = 0; i < 129; i++)
    {
        char *p = (char *)vm_map(nullptr, 0);
    }
    if (fork() == 0)
    {
        std::cout << "child created!" << std::endl;
        exit(0);
    }
    return 0;
}
