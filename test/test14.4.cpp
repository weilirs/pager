#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cassert>
#include "vm_app.h"

// test fork with written page then write and evict(only swap-backed pages)

int main()
{
    char *page1 = (char *)vm_map(nullptr, 0);
    char *page2 = (char *)vm_map(nullptr, 0);
    page1[0] = 'a';
    page2[0] = 'b';
    if (fork())
    {
        // This is the parent!
        page1[0] = 'a';
        page2[0] = 'b';
        vm_yield(); // Let the child run
    }
    else
    {
        // This is the child!
        page1[0] = 'b';
    }
}
