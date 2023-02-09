#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cassert>
#include "vm_app.h"

// Test Write after mapped then clock and swept and evict (swap-backed)

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

    // test read the zero pages will not incur eviction
    cout << page2[0] << endl;
    cout << page3[0] << endl;
    cout << page4[0] << endl;

    // write the pages to incur eviction (evict page 1)
    page2[0] = 'b';
    page3[0] = 'c';
    page4[0] = 'd';

    // Write the page2 again (page 2 should go back to written after mapped stage via vm_fault)
    page2[0] = 'b';

    // Read page2 to check
    cout << page2[0] << endl;

    return 0;
}