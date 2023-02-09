#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cassert>
#include "vm_app.h"

// Test read and write on evicted pages (swap-backed)

using std::cout, std::endl;

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

    // read the page1 again (page 1 should go back to physical memory and evict page2)
    cout << page1[0] << endl;

    // write page2 back to memory
    page2[0] = 'b';

    return 0;
}