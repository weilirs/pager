#include "vm_pager.h"
#include "vm_def.h"
#include "Pager.h"
#include <unistd.h>

#ifdef DEBUG
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#endif

// cppcheck-suppress unusedFunction

void vm_init(unsigned int memory_pages, unsigned int swap_blocks)
{
    // for debugging in local machine
#ifdef DEBUG
    DIR *env = opendir("./env");
    if (env)
    {
        closedir(env);
    }
    else if (errno == ENOENT)
    {
        chdir("../");
    }
    chdir("./env");
#endif
    Pager *ptr = new Pager(memory_pages, swap_blocks);
    pager.reset(ptr);
}

// cppcheck-suppress unusedFunction

int vm_create(pid_t parent_pid, pid_t child_pid)
{
    return pager->fork(parent_pid, child_pid);
}

// cppcheck-suppress unusedFunction

void vm_switch(pid_t pid)
{
    pager->switchTo(pid);
}

int vm_fault(const void *addr, bool write_flag)
{
    return pager->handleFault(addr, write_flag);
}

// cppcheck-suppress unusedFunction

void vm_destroy()
{
    pager->free();
}

// cppcheck-suppress unusedFunction

void *vm_map(const char *filename, unsigned int block)
{
    return pager->mapTo(filename, block);
}