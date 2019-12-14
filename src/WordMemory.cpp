#include <iostream>
#include "WordMemory.h"
#include <string.h>

using namespace std;

WordMemory::WordMemory()
{
    //init 1 Byte * 2048. A word = 4 Bytes. Word Address in [0, 4, 8, ... , 2044] [0-7FC], Max 2048/4 = 512 Instructions supported.
    memset(this->mem, 0, sizeof(this->mem));
}

WordMemory::~WordMemory()
{
}

void WordMemory::set(int address, uint32_t instuction)
{
    //write IMem
    this->mem[address] = (instuction >> 24) & 0xFF;     //bit 31-24
    this->mem[address + 1] = (instuction >> 16) & 0xFF; //bit 23 --16
    this->mem[address + 2] = (instuction >> 8) & 0xFF;  //bit 15 --8
    this->mem[address + 3] = instuction & 0xFF;         //bit 7-0
}

uint32_t WordMemory::get(int address)
{

    //read IMem
    uint32_t instrction_fectched = (this->mem[address] << 24) |
                                   (0x00FF0000 & (this->mem[address + 1] << 16)) |
                                   (0x0000FF00 & (this->mem[address + 2] << 8)) |
                                   (0x000000FF & this->mem[address + 3]);

    return (instrction_fectched);
}
