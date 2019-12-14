#include "latchReg.h"
#include <sstream>

#include <string.h>
#include <stdio.h>
#include <iostream>

//unsigned long int latchReg::PC;
unsigned long int latchReg::R[32];

latchReg::latchReg()
{
}

latchReg::~latchReg()
{
}

void latchReg::init()
{
    this->IR = 0;
    this->NPC = 0;
    this->A = 0;
    this->B = 0;
    this->Imm = 0;
    this->ALUOutput = 0;
    this->Cond = 0;
    this->LMD = 0;

    /* init share static R and PC in latchReg */
    //this->PC = 0;
    memset(this->R, 0, sizeof(this->R));
}

//set
void latchReg::setIR(unsigned long int v)
{
    this->IR = v;
};

void latchReg::setNPC(unsigned long int v)
{
    this->NPC = v;
};

void latchReg::setA(unsigned long long int v)
{
    this->A = v;
};

void latchReg::setB(unsigned long long int v)
{
    this->B = v;
};

void latchReg::setImm(unsigned long int v)
{
    this->Imm = v;
};

void latchReg::setALUOutput(unsigned long long int v)
{
    this->ALUOutput = v;
};

void latchReg::setCond(unsigned long int v)
{
    this->Cond = v;
};

void latchReg::setLMD(unsigned long int v)
{
    this->LMD = v;
};

// get
unsigned long int latchReg::getIR()
{
    return (this->IR);
};

unsigned long int latchReg::getNPC()
{
    return (this->NPC);
};

unsigned long long int latchReg::getA()
{
    return (this->A);
};

unsigned long long int latchReg::getB()
{
    return (this->B);
};

unsigned long int latchReg::getImm()
{
    return (this->Imm);
};

unsigned long long int latchReg::getALUOutput()
{
    return (this->ALUOutput);
};

unsigned long int latchReg::getCond()
{
    return (this->Cond);
};

unsigned long int latchReg::getLMD()
{
    return (this->LMD);
};

void latchReg::dumpReg32()
{
    //std::stringstream ss0;
    printf("\nREG:\n");

    for (int i = 0; i < 32; i += 4)
    {

        std::cout << "\t" << reg_as_asm(i) << " [r" << std::dec << i << "]: 0x";
        std::cout << std::hex << R[i] << "\t";

        for (int j = i + 1; j < i + 3; j++)
        {
            std::cout << "\t" << reg_as_asm(j) << " [r" << std::dec << j << "]: 0x";
            std::cout << std::hex << R[j] << "\t";
        }

        std::cout << reg_as_asm(i + 3) << " [r" << std::dec << i + 3 << "]: 0x";
        std::cout << std::hex << R[i + 3] << "\n";
    }

    std::cout << "\n\n";
}

std::string latchReg::reg_as_asm(int regid)
{
    std::string s_out;

    std::map<int, std::string> sreg;
    sreg[0] = "$zero";

    sreg[1] = "$at";
    sreg[2] = "$v0";
    sreg[3] = "$v1";
    sreg[4] = "$a0";
    sreg[5] = "$a1";
    sreg[6] = "$a2";
    sreg[7] = "$a3";
    sreg[8] = "$t0";
    sreg[9] = "$t1";

    sreg[10] = "$t2";
    sreg[11] = "$t3";
    sreg[12] = "$t4";
    sreg[13] = "$t5";
    sreg[14] = "$t6";
    sreg[15] = "$t7";
    sreg[16] = "$s0";
    sreg[17] = "$s1";
    sreg[18] = "$s2";
    sreg[19] = "$s3";
    sreg[20] = "$s4";
    sreg[21] = "$s5";
    sreg[22] = "$s6";
    sreg[23] = "$s7";
    sreg[24] = "$t8";
    sreg[25] = "$t9";
    sreg[26] = "$k0";
    sreg[27] = "$k1";
    sreg[28] = "$gp";
    sreg[29] = "$sp";
    sreg[30] = "$fp";
    sreg[31] = "$ra";

    return sreg[regid];
}