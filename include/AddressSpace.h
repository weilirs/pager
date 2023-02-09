#pragma once

#include "vm_def.h"
#include "DiskBlock.h"
#include <array>

class AddressSpace
{
public:
    page_table_t pageTable;

protected:
    std::array<const DiskBlock *, AddressSpaceSize> associatedDisk; // An array of disk block pointers in the disk
    unsigned int lowestInvalidIndex;                                // Keeps track of the next index

public:
    class Entry
    {
    public:
        page_table_entry_t &pte;
        const DiskBlock *&disk;
        Entry(page_table_entry_t &_pte, const DiskBlock *&_disk);
        Entry(const Entry &entry);
        bool resident(void) const;
        void reset(bool backedType);
    };

    template <typename T = char>
    class ConstPointer
    {
    protected:
        AddressSpace &space;

    public:
        const T *address;
        ConstPointer(AddressSpace &addr_space, const T *vptr = nullptr);
        ConstPointer(const ConstPointer &vptr);
        const T &operator*(void) const;
        ConstPointer operator++(int offset);
        ConstPointer operator--(int offset);
        std::ptrdiff_t operator-(const ConstPointer &vptr) const;
        ConstPointer operator+(std::ptrdiff_t _offset) const;
        const T &operator[](std::size_t _offset) const;
        operator bool(void) const;
        unsigned int index(void) const;
        unsigned long vpn(void) const;
        unsigned int offset(void) const;
        bool valid(void) const;
    };

    AddressSpace();
    int strcpy(std::unique_ptr<char[]> &ptr, ConstPointer<char> vptr);
    unsigned int nextVPNIndex(void) const;
    // requires to check for space beforehand
    void *allocate(void);
    bool full(void) const;
    Entry operator[](unsigned int index);
};
