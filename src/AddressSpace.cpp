#include "AddressSpace.h"
#include <stdexcept>
#include <cstring>

AddressSpace::Entry::Entry(page_table_entry_t &_pte, const DiskBlock *&_disk) : pte(_pte), disk(_disk) {}

AddressSpace::Entry::Entry(const AddressSpace::Entry &entry) : pte(entry.pte), disk(entry.disk) {}

// True if both disk and physical page are allocated
bool AddressSpace::Entry::resident(void) const
{
    return (bool)disk == (bool)pte.ppage;
}

void AddressSpace::Entry::reset(bool backedType)
{
    pte = {0, !backedType, 0};
    disk = nullptr;
}

template <typename T>
AddressSpace::ConstPointer<T>::ConstPointer(AddressSpace &addr_space, const T *vptr) : space(addr_space), address(vptr)
{
}

template <typename T>
AddressSpace::ConstPointer<T>::ConstPointer(const ConstPointer<T> &vptr) : space(vptr.space), address(vptr.address)
{
}

template <typename T>
const T &AddressSpace::ConstPointer<T>::operator*(void) const
{
    if (!valid() || VM_PAGESIZE - offset() < sizeof(T) || ((unsigned long)(address) & (alignof(T) - 1)))
    {
        throw std::runtime_error("Virtual address is invalid for this type, or the object will be on multiple pages!");
    }
    Entry pageInfo = space[index()];
    if (!pageInfo.pte.read_enable && vm_fault(address, false) != VM_SUCCESS)
    {
        throw std::runtime_error("Cannot handle fault!");
    }
    unsigned long ppn = pageInfo.pte.ppage;
    unsigned long physicalAddress = (ppn << OffsetBits) | offset();
    return *((const T *)(vm_physmem) + physicalAddress);
}

template <typename T>
AddressSpace::ConstPointer<T> AddressSpace::ConstPointer<T>::operator++(int _offset)
{
    ConstPointer ptr(*this);
    address += _offset ? _offset : 1;
    return ptr;
}

template <typename T>
AddressSpace::ConstPointer<T> AddressSpace::ConstPointer<T>::operator--(int _offset)
{
    ConstPointer ptr(*this);
    address -= _offset ? _offset : 1;
    return ptr;
}

template <typename T>
std::ptrdiff_t AddressSpace::ConstPointer<T>::operator-(const AddressSpace::ConstPointer<T> &vptr) const
{
    return address - vptr.address;
}

template <typename T>
AddressSpace::ConstPointer<T> AddressSpace::ConstPointer<T>::operator+(std::ptrdiff_t _offset) const
{
    ConstPointer temp(space, address + _offset);
    return temp;
}

template <typename T>
const T &AddressSpace::ConstPointer<T>::operator[](std::size_t _offset) const
{
    return *(this->operator+(_offset));
}

template <typename T>
AddressSpace::ConstPointer<T>::operator bool(void) const
{
    return valid();
}

template <typename T>
unsigned int AddressSpace::ConstPointer<T>::index(void) const
{
    return vpn() - ((unsigned long)VM_ARENA_BASEADDR >> OffsetBits);
}

template <typename T>
unsigned long AddressSpace::ConstPointer<T>::vpn(void) const
{
    return (unsigned long)address >> OffsetBits;
}

template <typename T>
unsigned int AddressSpace::ConstPointer<T>::offset(void) const
{
    return (unsigned long)address & mask(OffsetBits);
}

template <typename T>
bool AddressSpace::ConstPointer<T>::valid(void) const
{
    bool result = address >= VM_ARENA_BASEADDR && address <= (const char *)VM_ARENA_BASEADDR + VM_ARENA_SIZE;
    result = result && index() < space.nextVPNIndex();
    return result;
}

// Default constructor, set all values to 0
AddressSpace::AddressSpace() : pageTable(), associatedDisk(), lowestInvalidIndex(0)
{
    for (unsigned int vpnIndex = 0; vpnIndex < AddressSpaceSize; vpnIndex++)
    {
        pageTable.ptes[vpnIndex] = {0, 0, 0};
    }
}

int AddressSpace::strcpy(std::unique_ptr<char[]> &ptr, ConstPointer<char> vptr)
{
    std::string str_buf;
    char ch = 'a';
    while (ch != '\0')
    {
        try
        {
            ch = *vptr++;
        }
        catch (std::runtime_error &err)
        {
            return VM_FAILURE;
        }
        str_buf.push_back(ch);
    }
    char *str = new char[str_buf.length() + 1];
    std::strcpy(str, str_buf.c_str());
    ptr.reset(str);
    return VM_SUCCESS;
}

unsigned int AddressSpace::nextVPNIndex(void) const
{
    return lowestInvalidIndex;
}

void *AddressSpace::allocate(void)
{
    unsigned long vpn = lowestInvalidIndex++ + ((unsigned long)VM_ARENA_BASEADDR >> OffsetBits);
    return (void *)(vpn << OffsetBits);
}

bool AddressSpace::full(void) const
{
    return lowestInvalidIndex >= AddressSpaceSize;
}

AddressSpace::Entry AddressSpace::operator[](unsigned int index)
{
    return Entry(pageTable.ptes[index], associatedDisk[index]);
}

template class AddressSpace::ConstPointer<char>;
