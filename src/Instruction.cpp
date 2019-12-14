
#include <string>
#include <sstream>
#include <iostream>
#include <bitset>
#include <iomanip>

#include "Instruction.h"
#include <memory>
#include <map>
#include "latchReg.h"

using namespace std;

template <typename... Args>
std::string string_format(const std::string &format, Args... args)
{
    auto cstr_format = format.c_str();
    int size = std::snprintf(nullptr, 0, cstr_format, args...) + 1;
    std::unique_ptr<char[]> buffer(new char[size]);
    std::snprintf(buffer.get(), size, cstr_format, args...);
    return std::string(buffer.get(), buffer.get() + size - 1);
}

Instruction::Instruction(uint32_t inst)
{
    this->inst = inst;

    this->_op = -1;
    this->_type = -1;

    this->_funct = -1;
    this->_rs = -1;
    this->_rt = -1;
    this->_rd = -1;
    this->_shamt = -1;
    this->_offset = -1;
    this->_offset_hex_str;
    this->_address = -1;
    this->instValid = true;

    uint8_t _op = inst >> 26u;
    this->_op = _op;

    /*  func: ADD SUB AND  OR SLL  SRL  MUL*/

    if (_op == ALU_ZERO)
    {
        this->_type = R_TYPE;

        this->_rs = inst >> 21 & 0x1F;
        this->_rt = inst >> 16 & 0x1F;
        this->_rd = inst >> 11 & 0x1F;
        this->_shamt = inst >> 6 & 0x1F;
        this->_funct = inst & 0x3F;

        if ((this->_funct != ADD) &&
            (this->_funct != AND) &&
            (this->_funct != MUL) &&
            (this->_funct != OR) &&
            (this->_funct != SLL) &&
            (this->_funct != SRL) &&
            (this->_funct != SUB))
        {
            this->instValid = false;
            std::cout << "Instruction Not supported\n";
            dump_inst();
            exit(-1);
        }
    }
    /*ADDI,LUI ANDI ORI SLTI SLTIU LW SW*/
    else if (_op == ADDI ||
             _op == ANDI ||
             _op == LUI ||
             _op == ORI ||
             _op == SLTI ||
             _op == SLTIU ||
             _op == BEQ ||
             _op == LW ||
             _op == SW)
    {
        this->_type = I_TYPE;
        this->_rs = inst >> 21 & 0x1F;
        this->_rt = inst >> 16 & 0x1F;
        this->_offset = inst & 0xFFFFu;

        std::stringstream ss;
        ss << "0x" << std::hex << this->_offset;
        this->_offset_hex_str = ss.str();
    }
    else
    {
        this->instValid = false;
        std::cout << "Instruction Not supported\n";
        dump_inst();
        exit(-1);
    }
}

void Instruction::init()
{
    this->_inst_hex_str = this->as_hex();
    this->_inst_bin_str = this->as_bin();
    this->_inst_asm_str = this->as_asm();
}
void Instruction::dump_inst()
{
    std::cout << "INST: " + this->_inst_hex_str << "   ";
    std::cout << this->_inst_bin_str << "   ";
    std::cout << this->_inst_asm_str << std::endl;
}

void Instruction::set_nop()
{
    this->inst = NOPE;
}

bool Instruction::is_nop()
{
    return this->inst == NOPE;
}

std::string Instruction::as_hex()
{
    std::stringstream stream;

    stream << "0x" << setfill('0') << setw(8) << std::hex << this->inst;
    return stream.str();
}

std::string Instruction::as_bin()
{
    return std::bitset<32>(this->inst).to_string();
}

/* decode ASM format inst */
std::string Instruction::as_asm()
{
    std::string s_out;

    if (this->is_nop())
    {
        return "noop";
    }

    std::string reg_rs_s = latchReg{}.reg_as_asm(this->_rs);
    std::string reg_rd_s = latchReg{}.reg_as_asm(this->_rd);
    std::string reg_rt_s = latchReg{}.reg_as_asm(this->_rt);
    std::string reg_shamt_s = latchReg{}.reg_as_asm(this->_shamt);

    /*  func: ADD AND MUL OR SLL  SRL SUB  */
    if (this->_op == ALU_ZERO)
    {
        switch (this->_funct)
        {
        /* ALU Reg-Reg */
        case ADD:
            s_out = "add " + reg_rd_s + " " + reg_rs_s + " " + reg_rt_s;
            break;
        case AND:
            s_out = "and " + reg_rd_s + " " + reg_rs_s + " " + reg_rt_s;
            break;
        case MUL:
            s_out = "mul " + reg_rd_s + " " + reg_rs_s + " " + reg_rt_s;
            break;
        case OR:
            s_out = "or " + reg_rd_s + " " + reg_rs_s + " " + reg_rt_s;
            break;
        case SLL:
            s_out = "sll " + reg_rd_s + " " + reg_rt_s + " " + reg_shamt_s;
            break;
        case SRL:
            s_out = "srl " + reg_rd_s + " " + reg_rt_s + " " + reg_shamt_s;
            break;
        case SUB:
            s_out = "sub " + reg_rd_s + " " + reg_rs_s + " " + reg_rt_s;
            break;
        default:
            this->instValid = false;
        }
    }

    /*ADDI ANDI BEQ LUI LW ORI SLTI SLTIU SW*/
    else
    {

        switch (this->_op)
        {
        /* Branch*/
        case BEQ:
            s_out = "beq " + reg_rs_s + " " + reg_rt_s + " " + this->_offset_hex_str;
            break;

        /*Load and store*/
        case LW:
            s_out = "lw " + reg_rt_s + " " + std::to_string(this->_offset) + " " + reg_rs_s;
            break;

        case SW:
            s_out = "sw " + reg_rt_s + " " + std::to_string(this->_offset) + " " + reg_rs_s;
            break;

        /* ALU Reg-Imm */
        case ADDI:

            s_out = "addi " + reg_rt_s + " " + reg_rs_s + " " + this->_offset_hex_str;
            break;

        case ANDI:
            s_out = "andi " + reg_rt_s + " " + reg_rs_s + " " + this->_offset_hex_str;
            break;

        case LUI:
            s_out = "lui " + reg_rt_s + " " + this->_offset_hex_str;
            break;

        case ORI:
            s_out = "ori " + reg_rt_s + " " + reg_rs_s + " " + this->_offset_hex_str;
            break;

        case SLTI:
            s_out = "slti " + reg_rt_s + " " + reg_rs_s + " " + this->_offset_hex_str;
            break;
        case SLTIU:
            s_out = "sltiu " + reg_rt_s + " " + reg_rs_s + " " + this->_offset_hex_str;
            break;

        default:
            this->instValid = false;
        }
    }

    return s_out;
}

/*decode string ASM into uint32 instructoin */
uint32_t Instruction::as_inst(std::string asm_s)
{
    uint32_t a = 1;
    return a;
}