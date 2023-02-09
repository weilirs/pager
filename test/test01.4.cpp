#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout, std::endl;

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename = (char *)vm_map(nullptr, 0);

    /* Write the name of the file that will be mapped */
    strcpy(filename, "lampson83.txt");

    /* Map a page from the specified file */
    char *p = (char *)vm_map(filename, 0);

    // Map from the last mapped page
    char *p2 = (char *)vm_map(p, 0);

    /* Print the first part of the paper */
    for (unsigned int i = 0; i < 1937; i++)
    {
        cout << p[i];
    }
    cout << endl;

    for (unsigned int i = 0; i < 1937; i++)
        cout << p2[i];
    cout << endl;

    return 0;
}
