#include "Allocator.h"

Allocator::Allocator(unsigned int num_resource, const unsigned int &low) : resources(num_resource), reserved(0)
{
    std::iota(resources.rbegin(), resources.rend(), low); // Fill in the address range
}

// Check if resources are available
bool Allocator::hasResource(void) const 
{
    return resources.size() > 0 && reserved < resources.size();
}

// Returns the next available resource to be allocated
unsigned int Allocator::allocate(bool reservation) 
{
    assert(hasResource() || (reservation && reserved == resources.size()));
    unsigned int resource = resources.back();
    resources.pop_back();
    if (reservation)
    {
        assert(reserved > 0);
        reserved--;
    }
    return resource;
}

// Reserve resource
bool Allocator::reserve(void)
{
    if (hasResource())
    {
        reserved++;
        return true;
    }
    else
    {
        return false;
    }
}

// Cancel the reservation
bool Allocator::cancel(void)
{
    if (reserved > 0)
    {
        reserved--;
        return true;
    }
    else
    {
        return false;
    }
}

// Add the resouce back into resources
void Allocator::deallocate(const unsigned int &resource)
{
    resources.push_back(resource);
}

// Gets number of remaining resources
std::size_t Allocator::remainingResources(void) const
{
    return resources.size() - reserved;
}
