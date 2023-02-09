#pragma once

#include <cstdint>
#include <vector>
#include <numeric>
#include <cassert>

class Allocator
{
protected:
    std::vector<unsigned int> resources; // Vector of pages to be allocated
    std::size_t reserved; // Used before officially granting the memory, e.g. before copy-on-write

public:
    Allocator(unsigned int num_resource = 0, const unsigned int &low = 0);
    bool hasResource(void) const;
    unsigned int allocate(bool reservation = false);
    bool reserve(void);
    bool cancel(void);
    void deallocate(const unsigned int &resource);
    std::size_t remainingResources(void) const;
};
