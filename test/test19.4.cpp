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

    char *page1 = (char *)vm_map(nullptr, 0);
    page1[0] = 'a';

    /* Map a page from the specified file */
    char *p = (char *)vm_map(filename, 0);

    /* Print the first part of the paper */
    for (unsigned int i = 0; i < 1937; i++)
    {
        cout << p[i];
    }
    cout << endl;

    char *page2 = (char *)vm_map(nullptr, 0);
    page2[0] = 'a';

    if (fork())
    {
        vm_yield();
        char *p1 = (char *)vm_map(filename, 0);
        std::cout << p1[0] << std::endl;
    }
    else
    {
        char *p1 = (char *)vm_map(filename, 0);
        std::cout << p1[0] << std::endl;
        exit(0);
    }
    return 0;
}