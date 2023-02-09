#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cassert>
#include "vm_app.h"

using std::cout, std::endl;

// refer and evict dirty file-backed page
// Segfault 3/20

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename = (char *)vm_map(nullptr, 0);

    /* Write the name of the file that will be mapped */
    strcpy(filename, "lampson83.txt");

    /* Map a page from the specified file */
    char *p = (char *)vm_map(filename, 0);

    /* Print the first part of the paper */
    p[0] = 'a';

    char *page1 = (char *)vm_map(nullptr, 0);
    char *page2 = (char *)vm_map(nullptr, 0);
    page1[0] = 'b';
    page2[0] = 'c';

    // refer p[0] again
    cout << p[0];

    cout << page1[0];
    cout << page2[0];

    // evict p
    cout << filename[0];

    cout << endl;

    return 0;
}
