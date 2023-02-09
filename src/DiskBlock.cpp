#include "DiskBlock.h"
#include <cstring>

template <typename T, typename... Args>
inline void hash_combine(std::size_t &result, const T &arg, const Args &...args)
{
    hash_combine(result, arg);
    hash_combine(result, args...);
}

template <typename T>
inline void hash_combine(std::size_t &result, const T &arg)
{
    result ^= std::hash<T>{}(arg) + 0x9e3779b9 + (result << 6) + (result >> 2);
}

DiskBlock::DiskBlock(char *str, unsigned int _block) : _file(), file(), block(_block)
{
    set(str, _block);
}

DiskBlock::DiskBlock(std::unique_ptr<char[]> &str, unsigned int _block) : _file(), file(), block(_block)
{
    set(str, _block);
}

DiskBlock::DiskBlock(const DiskBlock &disk) : _file(), file(), block(0)
{
    if (disk.file.length())
    {
        this->_file.reset(new char[disk.file.length() + 1]);
        this->_file.get()[disk.file.length()] = '\0';
        disk.file.copy(this->_file.get(), disk.file.length());
        std::string_view temp(this->_file.get());
        this->file.swap(temp);
    }
    this->block = disk.block;
}

DiskBlock::DiskBlock(DiskBlock &&disk) : _file(std::move(disk._file)),
                                         file(std::move(disk.file)), block(std::move(disk.block))
{
}

void DiskBlock::set(char *str, unsigned int _block)
{
    clean();
    this->block = _block;
    this->_file.reset();
    if (str)
    {
        this->_file.reset(str);
        std::string_view temp(this->_file.get());
        this->file.swap(temp);
    }
}

void DiskBlock::set(std::unique_ptr<char[]> &str, unsigned int _block)
{
    clean();
    this->_file = std::move(str);
    this->block = _block;
    if (this->_file)
    {
        std::string_view temp(this->_file.get());
        this->file.swap(temp);
    }
}

void DiskBlock::clean(void)
{
    this->_file.reset();
    std::string_view temp;
    this->file.swap(temp);
    this->block = 0;
}

bool DiskBlock::isFileBacked(void) const
{
    return file.data();
}

DiskBlock &DiskBlock::operator=(const DiskBlock &block)
{
    DiskBlock temp(block);
    std::swap(this->_file, temp._file);
    std::swap(this->file, temp.file);
    std::swap(this->block, temp.block);
    return *this;
}

bool DiskBlock::operator==(const DiskBlock &other) const
{
    return this->file == other.file && this->block == other.block;
}

std::size_t std::hash<DiskBlock>::operator()(const DiskBlock &id) const
{
    std::size_t result = 0;
    hash_combine(result, id.file, id.block);
    return result;
}