#pragma once

#include "vm_pager.h"
#include <cassert>
#include <iostream>

#ifdef DEBUG
#define LOG(str) (std::cout << "[DEBUG]: " << (str) << std::endl)
#else
#define LOG(str)
#endif

constexpr int VM_SUCCESS = 0;
constexpr int VM_FAILURE = -1;

constexpr unsigned int mask(unsigned int width);

constexpr unsigned int VPNBits = 48;
constexpr unsigned int PPNBits = 20;
constexpr unsigned int OffsetBits = 16;
constexpr unsigned int AddressSpaceSize = VM_ARENA_SIZE / VM_PAGESIZE;

constexpr unsigned int mask(unsigned int width)
{
    return (0b1 << width) - 1;
}