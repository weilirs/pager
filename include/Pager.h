#pragma once

#include "Allocator.h"
#include "AddressSpace.h"
#include "Clock.h"
#include "Page.h"
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <deque>

class Pager
{
public:
    using FaultHandler = int (Pager::*)(unsigned int vpnIndex, [[maybe_unused]] unsigned int offset, bool write);

protected:
    // References and pointers to either key or data
    // stored in the container are only invalidated by erasing that element,
    // even when the corresponding iterator is invalidated.
    std::unordered_map<pid_t, AddressSpace> processTable;
    std::unordered_set<DiskBlock> diskBlocks;
    std::unordered_map<const DiskBlock *, RefererTable> diskBlockReferers;
    std::unordered_map<const DiskBlock *, unsigned int> activeFilePages;
    std::vector<Page> physicalPages;
    Allocator physicalPageAllocator;
    Allocator swapBlockAllocator;
    Clock clockSelector;
    FaultHandler handlerVectorTable[3];
    pid_t currentPID;

    // load and evict pages
    int evict(unsigned int ppn);
    int pageIn(unsigned int vpnIndex);
    const DiskBlock *record(DiskBlock &disk, unsigned int vpnIndex);
    // Returns allocated ppn
    unsigned int allocatePhysicalPage(void);
    int handleDiskFault(unsigned int vpnIndex, [[maybe_unused]] unsigned int offset, bool write);
    int handleReferenceFault(unsigned int vpnIndex, [[maybe_unused]] unsigned int offset, bool write);
    int handleFileWriteFault(unsigned int vpnIndex, [[maybe_unused]] unsigned int offset, bool write);
    int handleSwapWriteFault(unsigned int vpnIndex, [[maybe_unused]] unsigned int offset, bool write);
    int handleWriteFault(unsigned int vpnIndex, [[maybe_unused]] unsigned int offset, bool write);

public:
    Pager(unsigned int num_physical_page, unsigned int num_swap_block);
    void *mapTo(const char *file, unsigned int block);
    void switchTo(pid_t pid);
    // Fault Handlers: name them as handle*Fault
    int handleFault(const void *addr, bool write_flag);
    void free(void);
    int fork(pid_t parent, pid_t child);
};

extern std::unique_ptr<Pager> pager;
