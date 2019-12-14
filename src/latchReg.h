#ifndef LatchReg_H
#define LatchReg_H

#include <map>
#include <iterator>
#include <memory>
enum Reg
{
    $ZERO = 0, //The constant value 0

    $AT = 1, //Reserved for assembler

    $V0 = 2, //Values for results and expression evaluation
    $V1 = 3,

    $A0 = 4, //Arguments
    $A1 = 5,
    $A2 = 6,
    $A3 = 7,

    $T0 = 8, //Temporary registers
    $T1 = 9,
    $T2 = 10,
    $T3 = 11,
    $T4 = 12,
    $T5 = 13,
    $T6 = 14,
    $T7 = 15,

    $S0 = 16, //Saved registers
    $S1 = 17,
    $S2 = 18,
    $S3 = 19,
    $S4 = 20,
    $S5 = 21,
    $S6 = 22,
    $S7 = 23,

    $T8 = 24, //More temporary registers
    $T9 = 25,

    $K0 = 26, //Reserved for operating system kernel
    $K1 = 27,

    $GP = 28, //global pointer
    $SP = 29, //Stack pointer
    $FP = 30, //Frame pointer
    $RA = 31  //Return address

};

class latchReg
{
private:
    unsigned long int IR;
    unsigned long int NPC;
    unsigned long long int A;
    unsigned long long int B;
    unsigned long int Imm;
    unsigned long long int ALUOutput;
    unsigned long int Cond;
    unsigned long int LMD;

public:
    static unsigned long int R[32];
    //static unsigned long int PC;

    latchReg();
    ~latchReg();
    void init();

    unsigned long int getIR();
    unsigned long int getNPC();
    unsigned long long int getA();
    unsigned long long int getB();
    unsigned long int getImm();
    unsigned long long int getALUOutput();
    unsigned long int getCond();
    unsigned long int getLMD();

    void setIR(unsigned long int);
    void setNPC(unsigned long int);
    void setA(unsigned long long int);
    void setB(unsigned long long int);
    void setImm(unsigned long int);
    void setALUOutput(unsigned long long int);
    void setCond(unsigned long int);
    void setLMD(unsigned long int);

    void dumpReg32();

    std::string reg_as_asm(int); //input 0, output $zero.
};

#endif