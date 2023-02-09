#pragma once

#include "vm_def.h"
#include <unordered_map>
#include <deque>
#include <memory>
#include <string_view>

// Implements hash_combine of boost cpp
template <typename T, typename... Args>
inline void hash_combine(std::size_t &result, const T &arg, const Args &...args);

template <typename T>
inline void hash_combine(std::size_t &result, const T &arg);

class DiskBlock
{
protected:
    std::unique_ptr<char[]> _file;

public:
    std::string_view file;
    unsigned int block;

    DiskBlock(char *str = nullptr, unsigned int _block = 0);
    DiskBlock(std::unique_ptr<char[]> &str, unsigned int block);
    DiskBlock(const DiskBlock &disk);
    DiskBlock(DiskBlock &&disk);
    void set(char *str, unsigned int block);
    void set(std::unique_ptr<char[]> &str, unsigned int block);
    void clean(void);
    bool isFileBacked(void) const;
    DiskBlock &operator=(const DiskBlock &block);
    bool operator==(const DiskBlock &block) const;
};

template <>
class std::hash<DiskBlock>
{
public:
    std::size_t operator()(const DiskBlock &id) const;
};