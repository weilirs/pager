#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cassert>
#include "vm_app.h"

// test fork with written page (only swap-backed pages)
// Hanging after vm_destroy 3/20

int main()
{
    char *page1 = (char *)vm_map(nullptr, 0);
    page1[0] = 'a';
    if (fork())
    {
        // This is the parent!
        page1[0] = 'a';

        vm_yield(); // Let the child run
    }
    else
    {
        // This is the child!
        page1[0] = 'b';
    }
}
