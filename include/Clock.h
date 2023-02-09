#pragma once

#include "vm_def.h"
#include "Page.h"
#include <list>
#include <unordered_map>

class Clock
{
protected:
    std::unordered_map<const Page *, std::list<Page *>::const_iterator> table;
    std::list<Page *> evictor;

public:
    Clock();
    bool push(Page *page);
    bool erase(const Page *page);
    Page *pop(void);
    Page *select(void);
};