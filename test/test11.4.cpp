#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cassert>
#include "vm_app.h"

using std::cout, std::endl;

// test copying clock-out swap pages

int main()
{
    /* Allocate swap-backed page from the arena */
    char *page1 = (char *)vm_map(nullptr, 0);
    char *page2 = (char *)vm_map(nullptr, 0);
    char *page3 = (char *)vm_map(nullptr, 0);
    char *page4 = (char *)vm_map(nullptr, 0);

    /* Write the page */
    page1[0] = 'a';

    // write the pages to incur eviction (evict page 1)
    page2[0] = 'b';
    page3[0] = 'c';
    page4[0] = 'd';

    // page 2 and page 3 are not referenced
    if (fork())
    {
        vm_yield();
    }

    else
    {
        // evict page 2 with refCount = 2
        page1[0] = 'e';
    }

    return 0;
}