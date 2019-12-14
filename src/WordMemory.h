#ifndef Wordmemory_h
#define Wordmemory_h

#include <cstdint>
#include <iostream>

class WordMemory
{

public:
    int MAX_SIZE;
    uint32_t get(int);
    void set(int, uint32_t);
    WordMemory();
    ~WordMemory();

private:
    uint8_t mem[2048];
};

#endif