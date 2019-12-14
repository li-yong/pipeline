
/*
The simulator should implement the following instructions:
add, sub, addi, mul, lw, sw, beq, lui, and, andi, or, ori, sll, srl, slti, and sltiu



Type	Inst	Binary	ASM
R	add	0000 00ss ssst tttt dddd d000 0010 0000	add $d, $s, $t
R	sub	0000 00ss ssst tttt dddd d000 0010 0010	sub $d, $s, $t
I	addi	0010 00ss ssst tttt iiii iiii iiii iiii	addi $t, $s, imm
	mul		
I	lw	1000 11ss ssst tttt iiii iiii iiii iiii	lw $t, offset($s)
I	sw	1010 11ss ssst tttt iiii iiii iiii iiii	sw $t, offset($s)
J	beq	0001 00ss ssst tttt iiii iiii iiii iiii	beq $s, $t, offset
I	lui	0011 11-- ---t tttt iiii iiii iiii iiii	lui $t, imm
R	and	0000 00ss ssst tttt dddd d000 0010 0100	and $d, $s, $t
I	andi	0011 00ss ssst tttt iiii iiii iiii iiii	andi $t, $s, imm
R	or	0000 00ss ssst tttt dddd d000 0010 0101	or $d, $s, $t
I	ori	0011 01ss ssst tttt iiii iiii iiii iiii	ori $t, $s, imm
R	sll	0000 00ss ssst tttt dddd dhhh hh00 0000	sll $d, $t, h
R	srl	0000 00-- ---t tttt dddd dhhh hh00 0010	srl $d, $t, h
I	slti	0010 10ss ssst tttt iiii iiii iiii iiii	slti $t, $s, imm
I	sltiu	0010 11ss ssst tttt iiii iiii iiii iiii	sltiu $t, $s, imm
NA	noop	0000 0000 0000 0000 0000 0000 0000 0000	noop

			
R	mult	0000 00ss ssst tttt 0000 0000 0001 1000	mult $s, $t
R	multu	0000 00ss ssst tttt 0000 0000 0001 1001	multu $s, $t

 */

#ifndef instruction_h
#define instruction_h

const uint32_t NOPE = 0;

enum InstType
{
    R_TYPE = 0b00u,
    I_TYPE = 0b01u,
    J_TYPE = 0b10u,
};

enum OpCode
{
    ALU_ZERO = 0b000000u,
    /* Branch */
    BEQ = 0b000100u, //4

    /* Load and Store */
    LW = 0b100011u, //35
    SW = 0b101011u, //43

    /* ALU Reg-Imm */
    ADDI = 0b001000u,  //8
    ANDI = 0b001100u,  //12
    LUI = 0b001111u,   //15
    ORI = 0b001101u,   //13
    SLTI = 0b001010u,  //10
    SLTIU = 0b001011u, //11
};

enum FunctCode
{
    /* ALU Reg-Reg */
    ADD = 0b100000u, //32
    AND = 0b100100u, //36
    MUL = 0b011000u, //24 Assume the opcode and function code for mul to be same as that of mult

    OR = 0b100101u,  //37
    SLL = 0b000000u, //0
    SRL = 0b000010u, //2
    SUB = 0b100010u, //34

};

class Instruction
{
private:
    bool instValid;

public:
    uint32_t inst;

    int _op;
    int _type; //R, I , J

    int _funct; //R
    int _rs;
    int _rt;
    int _rd;
    int _shamt;       // R format
    uint16_t _offset; // I format

    uint32_t _address; //J format

    std::string _offset_hex_str;
    std::string _inst_hex_str;
    std::string _inst_bin_str;
    std::string _inst_asm_str;

    Instruction(uint32_t inst);

    void set_nop();

    bool is_nop();
    void init();

    std::string as_hex();

    std::string as_asm();

    std::string as_bin();

    uint32_t as_inst(std::string);

    void dump_inst();
};

std::string reg_name(uint8_t reg);

#endif