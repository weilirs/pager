#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

// This should cause the swap backed page to go to disk

int main()
{
    char *filename = (char *)vm_map(nullptr, 0);

    strcpy(filename, "lampson83.txt");

    char *p1 = (char *)vm_map(filename, 0);
    char *p2 = (char *)vm_map(filename, 0);
    char *p3 = (char *)vm_map(filename, 0);
    char *p4 = (char *)vm_map(filename, 0);

    for (unsigned int i = 0; i < 1937; i++)
    {
        std::cout << filename[i];
    }
    std::cout << std::endl;

    return 0;
}
