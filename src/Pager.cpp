#include "Pager.h"
#include <cstring>
#include <iostream>
using std::cout, std::endl;

Pager::Pager(unsigned int num_physical_page, unsigned int num_swap_block) : processTable(), diskBlocks(), diskBlockReferers(),
                                                                            activeFilePages(), physicalPages(),
                                                                            physicalPageAllocator(num_physical_page - 1, 1),
                                                                            swapBlockAllocator(num_swap_block, 0),
                                                                            clockSelector()
{
    physicalPages.reserve(num_physical_page);
    char *physmem = (char *)vm_physmem;
    for (unsigned int ppn = 0; ppn < num_physical_page; ppn++) // Create of a vector of memory pages
    {
        physicalPages.emplace_back(physmem);
        physmem += VM_PAGESIZE;
    }
    physicalPages[0].clean(); // Configure first page
    physicalPages[0].referenced = true;
    // put in handlers into vector table
    handlerVectorTable[0] = &Pager::handleReferenceFault;
    handlerVectorTable[1] = &Pager::handleWriteFault;
    handlerVectorTable[2] = &Pager::handleDiskFault;
}

int Pager::evict(unsigned int ppn)
{
    Page &page = physicalPages[ppn];
    const DiskBlock *disk = page.disk;
    int result = page.evict(); // Will be either VM_SUCCESS or VM_FAILURE
    if (result != VM_SUCCESS)
    {
        return result;
    }
    if (disk->isFileBacked()) // Needed to take care of filebacked pages
    {
        activeFilePages.erase(disk);
    }
    physicalPageAllocator.deallocate(ppn);
    return result;
}

int Pager::pageIn(unsigned int vpnIndex)
{
    unsigned int ppn = allocatePhysicalPage();
    const DiskBlock *disk = processTable[currentPID][vpnIndex].disk;
    physicalPages[ppn].mapTo(disk, diskBlockReferers);
    int result = physicalPages[ppn].load(); // Will be either VM_SUCCESS or VM_FAILURE
    if (result != VM_SUCCESS)
    {
        physicalPageAllocator.deallocate(ppn);
        physicalPages[ppn].reset();
        return result;
    }
    if (disk->isFileBacked())
    {
        activeFilePages[disk] = ppn;
    }
    return result;
}

const DiskBlock *Pager::record(DiskBlock &disk, unsigned int vpnIndex)
{
    AddressSpace &space = processTable[currentPID];
    AddressSpace::Entry entry(space[vpnIndex]);
    auto iter = diskBlocks.find(disk);
    if (iter == diskBlocks.end())
    {
        iter = diskBlocks.emplace_hint(iter, std::move(disk));
    }
    const DiskBlock *diskKey = &*iter;
    entry.disk = diskKey;
    diskBlockReferers[diskKey][currentPID].push_back(&entry.pte);
    return diskKey;
}

unsigned int Pager::allocatePhysicalPage(void)
{
    if (!physicalPageAllocator.hasResource())
    {
        Page *victim = clockSelector.select();
        evict(victim->ppn());
    }
    unsigned int ppn = physicalPageAllocator.allocate();
    clockSelector.push(&physicalPages[ppn]);
    return ppn;
}

void *Pager::mapTo(const char *file, unsigned int block)
{
    int result = VM_SUCCESS;
    AddressSpace &space = processTable[currentPID];
    std::unique_ptr<char[]> file_name;
    if (file)
    {
        AddressSpace::ConstPointer<char> vptr(space, file);
        result = space.strcpy(file_name, vptr);
    }
    if (result == VM_FAILURE || (!file && !swapBlockAllocator.hasResource()) || space.full())
    {
        return nullptr;
    }
    // address must be allocated after no failure is ensured
    unsigned int vpnIndex = space.nextVPNIndex();
    void *vaddr = space.allocate();
    AddressSpace::Entry entry(space[vpnIndex]);
    entry.reset(file);
    if (file)
    {
        DiskBlock disk(file_name, block);
        const DiskBlock *diskKey = record(disk, vpnIndex);
        auto itt = activeFilePages.find(diskKey);
        if (itt != activeFilePages.end())
        {
            physicalPages[itt->second].sync(entry.pte);
        }
    }
    else
    {
        swapBlockAllocator.reserve();
    }
    return vaddr;
}

void Pager::switchTo(pid_t pid)
{
    currentPID = pid;
    page_table_base_register = &processTable[pid].pageTable;
}

int Pager::handleFault(const void *addr, bool write_flag)
{
    AddressSpace &space = processTable[currentPID];
    AddressSpace::ConstPointer<char> vptr(space, (const char *)addr);
    if (!vptr)
    {
        return VM_FAILURE;
    }
    int code = (!space[vptr.index()].resident()) << 1;
    if (!code)
    {
        Page &page = physicalPages[space[vptr.index()].pte.ppage];
        code |= page.referenced;
    }
    return (this->*(handlerVectorTable[code]))(vptr.index(), vptr.offset(), write_flag);
}

int Pager::handleDiskFault(unsigned int vpnIndex, [[maybe_unused]] unsigned int offset, bool write_flag)
{
    int result = pageIn(vpnIndex);
    if (result == VM_SUCCESS && write_flag)
    {
        handleWriteFault(vpnIndex, offset, write_flag);
    }
    return result;
}

int Pager::handleReferenceFault(unsigned int vpnIndex, [[maybe_unused]] unsigned int offset, bool write_flag)
{
    AddressSpace &space = processTable[currentPID];
    AddressSpace::Entry entry(space[vpnIndex]);
    if (write_flag)
    {
        handleWriteFault(vpnIndex, offset, write_flag);
    }
    physicalPages[entry.pte.ppage].referenced = true;
    physicalPages[entry.pte.ppage].broadcast();
    return VM_SUCCESS;
}

int Pager::handleFileWriteFault(unsigned int vpnIndex, [[maybe_unused]] unsigned int offset, bool write_flag)
{
    AddressSpace &space = processTable[currentPID];
    AddressSpace::Entry entry(space[vpnIndex]);
    physicalPages[entry.pte.ppage].dirty = true;
    physicalPages[entry.pte.ppage].broadcast();
    return VM_SUCCESS;
}

int Pager::handleSwapWriteFault(unsigned int vpnIndex, [[maybe_unused]] unsigned int offset, bool write)
{
    AddressSpace &space = processTable[currentPID];
    AddressSpace::Entry entry(space[vpnIndex]);
    if (!entry.pte.ppage)
    {
        unsigned int block = swapBlockAllocator.allocate(true);
        DiskBlock disk(nullptr, block);
        const DiskBlock *diskKey = record(disk, vpnIndex);
        unsigned int ppn = allocatePhysicalPage();
        entry.pte.ppage = ppn;
        physicalPages[ppn].clean();
        physicalPages[ppn].mapTo(diskKey, diskBlockReferers);
        physicalPages[ppn].referenced = true;
    }
    physicalPages[entry.pte.ppage].dirty = true;
    physicalPages[entry.pte.ppage].broadcast();
    return VM_SUCCESS;
}

int Pager::handleWriteFault(unsigned int vpnIndex, [[maybe_unused]] unsigned int offset, bool write)
{
    AddressSpace &space = processTable[currentPID];
    AddressSpace::Entry entry(space[vpnIndex]);
    bool backedType = entry.disk && entry.disk->isFileBacked();
    if (backedType)
    {
        return handleFileWriteFault(vpnIndex, offset, write);
    }
    else
    {
        return handleSwapWriteFault(vpnIndex, offset, write);
    }
}

void Pager::free(void)
{
    AddressSpace &space = processTable[currentPID];
    for (unsigned int vpnIndex = 0; vpnIndex < space.nextVPNIndex(); vpnIndex++)
    {
        AddressSpace::Entry entry(space[vpnIndex]);
        if (entry.disk && entry.disk->isFileBacked())
        {
            // File-backed resident page
            RefererTable &referers = diskBlockReferers[entry.disk];
            auto iter = referers.find(currentPID);
            if (iter != referers.end())
            {
                referers.erase(iter);
            }
        }
        else if (entry.pte.ppage)
        {
            // Swap-backed resident page
            const DiskBlock *disk = entry.disk;
            unsigned int ppn = entry.pte.ppage;
            swapBlockAllocator.deallocate(disk->block);
            physicalPageAllocator.deallocate(ppn);
            clockSelector.erase(&physicalPages[ppn]);
            physicalPages[ppn].reset();
            diskBlockReferers.erase(disk);
            diskBlocks.erase(*disk);
        }
        else if (entry.resident())
        {
            // Swap-backed zero page
            swapBlockAllocator.cancel();
        }
        else
        {
            // Swap-backed non-resident page
            const DiskBlock *disk = entry.disk;
            swapBlockAllocator.deallocate(disk->block);
            diskBlockReferers.erase(disk);
            diskBlocks.erase(*disk);
        }
    }
    processTable.erase(currentPID);
}

int Pager::fork(pid_t parent, pid_t child)
{
    // need modification
    if (!swapBlockAllocator.hasResource())
    {
        return VM_FAILURE;
    }
    processTable[child] = AddressSpace();
    return VM_SUCCESS;
}

std::unique_ptr<Pager> pager;
