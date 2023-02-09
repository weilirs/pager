#include "Clock.h"

Clock::Clock() : table(), evictor() {}

// Pushes a page into a data structure to ensure participation in the clock algorithm
bool Clock::push(Page *page)
{
    evictor.push_back(page);
    return table.insert({page, --evictor.cend()}).second;
}

// Used in evictions
bool Clock::erase(const Page *page)
{
    auto iter = table.find(page);
    if (iter != table.cend())
    {
        evictor.erase(iter->second);
        table.erase(iter);
        return true;
    }
    return false;
}

Page *Clock::pop(void)
{
    Page *victim = evictor.front();
    erase(victim);
    return victim;
}

// Selects a page to be evicted
Page *Clock::select(void)
{
    Page *victim = nullptr;
    while (!victim)
    {
        Page *next = pop();
        if (next->referenced)
        {
            next->referenced = false;
            next->broadcast();
            push(next);
        }
        else
        {
            victim = next;
        }
    }
    return victim;
}
