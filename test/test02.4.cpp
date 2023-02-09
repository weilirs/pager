#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

// Test sharing of file-backed pages and swap-backed pages

int main(int argc, char *argv[])
{
    char *file_name = (char *)vm_map(nullptr, 0);
    strcpy(file_name, "lampson83.txt");
    char *file = (char *)vm_map(file_name, 0);
    file[0] = 'h';
    pid_t child_pid = fork();
    if (child_pid == 0)
    {
        char *child_file_name = (char *)vm_map(nullptr, 0);
        strcpy(child_file_name, "lampson83.txt");
        std::cout << file_name << ' ' << child_file_name << std::endl;
        char *child_file = (char *)vm_map(child_file_name, 0);
        std::cout << child_file[0] << file[0] << std::endl;
        child_file[1] = 'I';
        vm_yield();
        file[2] = 'N';
        strcpy(file_name, "data1.bin");
        std::cout << file_name << std::endl;
        exit(0);
    }
    else
    {
        vm_yield();
        std::cout << file[1] << file[2] << std::endl;
        vm_yield();
        std::cout << file[1] << file[2] << std::endl;
        std::cout << file_name << std::endl;
    }
    return 0;
}