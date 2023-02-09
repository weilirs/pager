#include "Page.h"
#include <cstring>

Page::Page(void *physmem) : disk(nullptr), referers(nullptr), memory(physmem), dirty(false), referenced(false) {}

void Page::mapTo(const DiskBlock *_disk, std::unordered_map<const DiskBlock *, RefererTable> &hashTable)
{
    disk = _disk;
    referers = &hashTable[disk];
}

int Page::load(void)
{
    assert(disk);
    if (file_read(disk->file.data(), disk->block, memory) != VM_SUCCESS) // Invalid file read
    {
        return VM_FAILURE;
    }
    referenced = true;
    dirty = false;
    broadcast();
    return VM_SUCCESS;
}

int Page::evict(void)
{
    assert(disk);
    if (dirty && file_write(disk->file.data(), disk->block, memory) != VM_SUCCESS) // Invalid file write, dirty file but unable to write changes to disk
    {
        return VM_FAILURE;
    }
    broadcast(true);
    reset();
    return VM_SUCCESS;
}

void Page::sync(page_table_entry_t &entry, bool evict_flag)
{
    entry.ppage = (evict_flag) ? (0) : (ppn());
    entry.read_enable = !evict_flag && referenced;
    entry.write_enable = !evict_flag && referenced && dirty;
}

void Page::broadcast(bool evict_flag)
{
    for (auto &kvp : *referers)
    {
        for (page_table_entry_t *entry : kvp.second)
        {
            sync(*entry, evict_flag);
        }
    }
}

void Page::reset(void)
{
    disk = nullptr;
    referers = nullptr;
    dirty = false;
    referenced = false;
}

void Page::clean(void)
{
    reset();
    std::memset(memory, 0, VM_PAGESIZE);
}

unsigned int Page::ppn(void) const
{
    return ((char *)memory - (char *)vm_physmem) >> OffsetBits;
}

Page::~Page()
{
    reset();
}
