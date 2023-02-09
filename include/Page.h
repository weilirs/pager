#pragma once

#include "vm_def.h"
#include "DiskBlock.h"
#include <unordered_map>
#include <deque>

using RefererTable = std::unordered_map<pid_t, std::deque<page_table_entry_t *>>;

class Page
{
public:
    const DiskBlock *disk; // Pages get mapped to DiskBlock objects
    RefererTable *referers; // Map from pid to page table entries
    void *const memory;
    bool dirty : 1;
    bool referenced : 1;

    Page() = delete;
    Page(void *physmem);
    // write to disk and referers must go throught this interface
    void mapTo(const DiskBlock *disk, std::unordered_map<const DiskBlock *, RefererTable> &hashTable);
    // load and evict is called when it is not free (disk != nullptr)
    int load(void);
    int evict(void);
    // sync to one entry, useful in vm_map
    void sync(page_table_entry_t &entry, bool evict_flag = false);
    // to be called after changing bits
    void broadcast(bool evict_flag = false);
    void reset(void);
    void clean(void);
    unsigned int ppn(void) const;
    ~Page();
};
