#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

// Testing the file backed page on exit

using std::cout, std::endl;

int main()
{
    if (fork())
    {
        vm_yield();
        char *filename = (char *)vm_map(nullptr, 0);
        strcpy(filename, "lampson83.txt");
        char *p = (char *)vm_map(filename, 0);
        std::cout << p[0] << std::endl;
    }
    else
    {
        char *filename = (char *)vm_map(nullptr, 0);
        strcpy(filename, "lampson83.txt");
        char *p = (char *)vm_map(filename, 0);
        p[0] = '.';
        std::cout << p[0] << std::endl;
        exit(0);
    }
    return 0;
}
