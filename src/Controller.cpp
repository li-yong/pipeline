#include <iostream>
#include <cctype>
#include <string.h>
#include <stdio.h>

#include "Controller.h"
#include "latchReg.h"
#include "Instruction.h"
#include "util.h"

using namespace std;

Controller::Controller()
{
}

Controller::~Controller()
{
}

void Controller::init(WordMemory pWM, std::string mode, int number_to_run, int inst_cnt_all, bool verbose, bool interactive)
{

    this->mode = mode;
    this->number_to_run = number_to_run;
    this->inst_cnt_all = inst_cnt_all;
    this->inst_cnt_left = inst_cnt_all;
    this->inst_cnt_run = 0;
    this->inst_cnt_skip = 0;
    this->inst_fetched_cnt = 0;

    this->pc = 0;
    this->ppc = 0;

    this->clock = 1; //clock start from 1. to comprimised with cycles user inputed.

    ID_hit = 0;
    EX_hit = 0;
    IF_hit = 0;
    MEM_hit = 0;
    WB_hit = 0;

    this->inst_memory = pWM;
    this->data_memory = WordMemory{};

    this->verbose = verbose;
    this->interactive = interactive;

    IFID = latchReg{};
    IDEX = latchReg{};
    EXMEM = latchReg{};
    MEMWB = latchReg{};

    IFID_n = latchReg{};
    IDEX_n = latchReg{};
    EXMEM_n = latchReg{};
    MEMWB_n = latchReg{};

    IFID.init();
    IDEX.init();
    EXMEM.init();
    MEMWB.init();

    IFID_n.init();
    IDEX_n.init();
    EXMEM_n.init();
    MEMWB_n.init();

    //insert_nop = 0; //false

    hazard_data_raw_cnt = 0;
    hazard_ctrl_cnt = 0;

    branch = 0;
    memset(R_to_be_read, 0, sizeof(R_to_be_read));
    memset(R_to_be_write, 0, sizeof(R_to_be_write));
}

int Controller::ask_number_of_run(string mode)
{
    if (interactive)
    {
        int n;
        std::cout << "\nHow many " << mode << "s to be run? " << inst_cnt_left << " instructions left. (0. dump then exit):  ";
        std::cin >> n;
        return n;
    }
}

int Controller::ask_dump_option()
{

    int n;
    std::cout << "\nShow dump? 0.No, 1.Reg+Latch+Statistic, 2.Inst Memeory, 3.Data Memory.  :  ";
    std::cin >> n;

    while (1)
    {
        if (n != 0 && n != 1 && n != 2 && n != 3)
        {
            cout << "wrong input. " << n << ". valid input [0,1,2,3]\n";
            n = ask_dump_option();
        }
        else
        {
            break;
        }
    }
    return n;
}

void Controller::dump_all()
{
    while (1)
    {
        auto n = ask_dump_option();
        if (n == 0)
        {
            break;
        }
        else if (n == 1)
        {
            cout << "Dump Reg, Latch, Statistic\n";
            latchReg{}.dumpReg32();
            cout << "\npc(next inst to be fetched):\t" << std::dec << pc << std::endl;
            cout << "next inst index :\t" << std::dec << pc / 4 + 1 << std::endl;

            cout << endl;
            dump_IFID();
            dump_IDEXE();
            dump_EXMEM();
            dump_MEMWB();

            cout << endl;
            statistic();
        }
        else if (n == 2)
        {
            cout << "dump inst memory:\n";
            DumpMemory(inst_memory);
        }
        else if (n == 3)
        {
            cout << "dump data memory:\n";
            DumpMemory(data_memory);
        }
        else
        {
            break;
        }
    }
}

uint32_t Controller::run()
{
    std::string CYCLE = "cycle";
    std::string INST = "inst";

    bool inst_mode_stop_fecth = false;
    int inst_fetch_cnt = 1;

    int number_to_run_cycle = 0;
    int number_to_run_inst = 0;
    int clock_cnt = 0;

    if (interactive)
    {

        if (CYCLE.compare(mode) == 0)
        {
            number_to_run_cycle = ask_number_of_run(mode);
        }
        else if (INST.compare(mode) == 0)
        {
            number_to_run_inst = ask_number_of_run(mode);
            number_to_run = number_to_run_inst;
        }
    }

    while (1)
    {

        /* user input start */
        if (verbose)
        {
            cout << "\n\n"
                 << "---- clock " << clock << " ----" << endl;
            cout << "inst_cnt_run " << inst_cnt_run << endl;
            cout << "inst_cnt_all " << inst_cnt_all << endl;
            cout << "inst_cnt_skip " << inst_cnt_skip << endl;
            cout << "inst_cnt_left " << inst_cnt_left << endl;
            cout << "pc " << pc << endl;
        }

        if (inst_cnt_left == 0)
        {
            cout << "\n\nNo more instructions left. Have run " << inst_cnt_run << " instructionis in " << clock - 1 << " cycles." << endl;
            cout << "now dump and exit:\n";
            dump_all();
            cout << "exit as no more instruction to run.\n";
            exit(0);
        }

        if ((CYCLE.compare(mode) == 0) && (clock_cnt >= number_to_run_cycle))
        {
            cout << clock_cnt << " cycles have been run."
                 << "current clock " << clock
                 << ". currrent pc " << pc << endl;

            if (interactive)
            {
                dump_all();
                number_to_run_cycle = ask_number_of_run("cycle");

                if (number_to_run_cycle == 0)
                {
                    dump_all();
                    cout << "exit as user request.\n";
                    exit(0);
                }
                clock_cnt = 0;
            }
            else
            {
                dump_all();
                exit(0);
            }
        }

        if ((INST.compare(mode) == 0))
        {
            if (inst_cnt_run + inst_cnt_skip >= number_to_run)
            {
                cout << "\n"
                     << inst_cnt_run << " instructions have been run. "
                     << inst_cnt_skip << " instructions were skipped. "
                     << inst_cnt_left << " instrctions left. next IF will fetch inst at number " << (pc - 4) / 4 << endl;

                if (interactive)
                {
                    dump_all();

                    number_to_run = ask_number_of_run("instruction");

                    if ((number_to_run == 0))
                    {
                        dump_all();
                        cout << "exit as user request.\n"
                             << endl;
                        exit(0);
                    }

                    if (number_to_run > inst_cnt_left)
                    {
                        number_to_run = inst_cnt_left;
                    }

                    inst_cnt_run = 0;
                    inst_cnt_skip = 0;
                    //inst_fetched_cnt = 0;
                }
                else
                {
                    cout << "Dump Reg, Latch, Statistic\n";
                    latchReg{}.dumpReg32();
                    cout << "\npc(next inst to be fetched):\t" << std::dec << pc << std::endl;
                    cout << "next inst index :\t" << std::dec << pc / 4 + 1 << std::endl;

                    cout << endl;
                    dump_IFID();
                    dump_IDEXE();
                    dump_EXMEM();
                    dump_MEMWB();

                    cout << endl;
                    statistic();
                    exit(0);
                }
            }
        }

        /* IF Stage */
        IF();

        /* ID Stage */
        ID();
        IFID = IFID_n;

        /* EXE Stage */
        EX();
        IDEX = IDEX_n;

        /*MEM Stage */
        MEM();
        EXMEM = EXMEM_n;

        /* WB Stage */
        WB();
        MEMWB = MEMWB_n;

        clock += 1;
        clock_cnt += 1;
    }
}
/***********************************************/
/*   IF STAGE  */
/***********************************************/
void Controller::IF()
{
    stage = "IF";
    ppc = pc; //keep current pc.

    std::string INST = "inst";
    uint32_t inst_if;

    /*control hazard */

    if (hazard_data_raw_cnt > 0)
    {
        inst_if = NOPE;
    }
    else if (hazard_ctrl_cnt > 0)
    {
        inst_if = NOPE;
    }
    else
    {
        if (EXMEM.getCond() == 1)
        {
            pc = EXMEM.getALUOutput();

            if (verbose)
            {
                cout << "IF" << clock << ": pc moved to  " << pc << " as previous beq asked. " << endl;
            }
            EXMEM_n.setCond(0);
        }

        if (pc > 4 * (inst_cnt_all - 1))
        {
            //all instructions has been fetched
            inst_if = NOPE;
        }
        else
        {

            inst_if = inst_memory.get(pc);
            IF_hit++;
            IFID_n.setNPC(pc + 4);
            pc += 4;
            inst_fetched_cnt++;
        }
    }

    IFID_n.setIR(inst_if);

    if (verbose)
    {
        Instruction pinst_if(inst_if);
        pinst_if.init();
        instr_asm = pinst_if._inst_asm_str;
        cout << "\n"
             << stage << clock << ":" << instr_asm << " pc " << pc << ", the inst index " << pc / 4 << endl;
    }
}

/***********************************************/
/*   ID STAGE  */
/***********************************************/
void Controller::ID()
{
    stage = "ID";
    auto inst_id = IFID.getIR();
    Instruction pinst_id(inst_id);
    pinst_id.init();
    instr_asm = pinst_id._inst_asm_str;

    if (verbose)
    {
        cout << stage << clock << ":" << instr_asm << endl;
    }

    auto rs = pinst_id._rs;
    auto rt = pinst_id._rt;
    auto rd = pinst_id._rd;
    auto op = pinst_id._op;
    auto type = pinst_id._type;
    auto funct = pinst_id._funct;

    //lock the reg for current inst
    _lock_reg(inst_id);

    IDEX_n.setA((uint64_t)IFID.R[rs]); //read register at the 2nd half of CC.
    IDEX_n.setB((uint64_t)IFID.R[rt]);

    IDEX_n.setNPC(IFID.getNPC());
    IDEX_n.setIR(IFID.getIR());
    IDEX_n.setImm((int64_t)pinst_id._offset); //sign-extend

    if (!pinst_id.is_nop())
    {
        ID_hit++;
    }

    //control hazard
    if ((pinst_id._type == I_TYPE) && (pinst_id._op == BEQ))
    {
        hazard_ctrl_cnt++;
        IFID_n.setIR(NOPE);
        pc = ppc; //rewind to previous pc.
    }

    //data hazard

    if ((hazard_data_raw_cnt == 0) && (clock > 1) && peep_data_hazard(IFID_n.getIR()))
    {
        hazard_data_raw_cnt++;
        IFID_n.setIR(NOPE);
        pc = ppc; //rewind to previous pc.
    }
    else
    {
        hazard_data_raw_cnt = 0;
    }
}

/***********************************************/
/*   EX STAGE  */
/***********************************************/
void Controller::EX()
{
    stage = "EX";
    auto inst_ex = IDEX.getIR();
    Instruction pinst_ex(inst_ex);
    pinst_ex.init();

    instr_asm = pinst_ex._inst_asm_str;

    if (verbose)
    {
        cout << stage << clock << ":" << instr_asm << endl;
    }

    if (pinst_ex.is_nop())
    {
        inst_ex = NOPE;
    }
    else
    {
        EX_hit++;

        EXMEM_n.setCond(0); //init to 0 before ALU.
        auto out = ALU(inst_ex);
        EXMEM_n.setALUOutput(out);
    }

    if (verbose)
    {
        cout << stage << clock << ": unlock reg " << instr_asm << endl;
    }
    _unlock_reg(inst_ex);
    EXMEM_n.setIR(inst_ex);

} //end of EX

/***********************************************/
/*   MEM STAGE  */
/***********************************************/
void Controller::MEM()
{
    stage = "MEM";
    auto inst_mem = EXMEM.getIR();
    Instruction pinst_mem(inst_mem);
    pinst_mem.init();
    instr_asm = pinst_mem._inst_asm_str;
    if (verbose)
    {
        cout << stage << clock << ":" << instr_asm << endl;
    }

    MEMWB_n.setIR(inst_mem);

    auto itype = pinst_mem._type;
    auto op = pinst_mem._op;
    auto funct = pinst_mem._funct;
    auto rs = pinst_mem._rs;
    auto rt = pinst_mem._rt;
    auto rd = pinst_mem._rd;

    if ((itype == R_TYPE) && (!pinst_mem.is_nop()))
    {
        MEMWB_n.setALUOutput(EXMEM.getALUOutput());

        if ((op == ALU_ZERO) &&
            ((funct == ADD) ||
             (funct == AND) ||
             (funct == OR) ||
             (funct == SLL) ||
             (funct == SRL) ||
             (funct == SUB)))
        {
            MEMWB_n.R[rd] = EXMEM.getALUOutput(); //update reg in advance
        }
        else if ((op == ALU_ZERO) && (funct == MUL))
        {
            unsigned long long int tmp = EXMEM.getALUOutput();

            uint32_t msb = tmp >> 32 & 0xFFFFFFFF;
            uint32_t lsb = tmp & 0xFFFFFFFF;

            MEMWB_n.R[rd + 1] = msb;
            MEMWB_n.R[rd] = lsb;
        }
    }
    else if (itype == I_TYPE)
    {

        if (pinst_mem._op == LW)
        {
            MEMWB_n.setLMD(data_memory.get(EXMEM.getALUOutput()));
            MEM_hit++;
        }
        else if (pinst_mem._op == SW)
        {
            data_memory.set(EXMEM.getALUOutput(), EXMEM.getB());
            MEM_hit++;
        }
        else if ((op == ADDI) ||
                 (op == ANDI) ||
                 (op == LUI) ||
                 (op == ORI) ||
                 (op == SLTI) ||
                 (op == SLTIU))
        {
            MEMWB_n.setALUOutput(EXMEM.getALUOutput());
            MEMWB_n.R[rt] = EXMEM.getALUOutput();
        }
        else if ((op == BEQ))
        {
            //No MEM stage
        }
    }

} //end of MEM

/***********************************************/
/*   WB STAGE  */
/***********************************************/
void Controller::WB()
{

    auto inst_wb = MEMWB.getIR();
    Instruction pinst_wb(inst_wb);
    pinst_wb.init();

    stage = "WB";
    instr_asm = pinst_wb._inst_asm_str;

    if (verbose)
    {
        cout << stage << clock << ":" << instr_asm << endl;
    }

    auto itype = pinst_wb._type;

    auto op = pinst_wb._op;
    auto rs = pinst_wb._rs;
    auto rt = pinst_wb._rt;
    auto rd = pinst_wb._rd;
    auto funct = pinst_wb._funct;

    /* processing */
    if ((itype == R_TYPE) && (!pinst_wb.is_nop()))
    {

        if ((op == ALU_ZERO) &&
            ((funct == ADD) ||
             (funct == AND) ||
             (funct == OR) ||
             (funct == SLL) ||
             (funct == SRL) ||
             (funct == SUB)))
        {
            /* reg are write in MEM*/
            WB_hit++;
            inst_cnt_run++;
            inst_cnt_left--;
        }
        else if ((op == ALU_ZERO) && (funct == MUL))
        {
            WB_hit++;
            inst_cnt_run++;
            inst_cnt_left--;
        }
    }

    if (itype == I_TYPE)
    {

        if ((op == ADDI) ||
            (op == ANDI) ||
            (op == LUI) ||
            (op == ORI) ||
            (op == SLTI) ||
            (op == SLTIU))
        {
            WB_hit++;
            inst_cnt_run++;
            inst_cnt_left--;
        }
        else if (op == LW)
        {
            WB_hit++;
            inst_cnt_run++;
            inst_cnt_left--;

            MEMWB_n.R[rt] = MEMWB.getLMD();
        }
        else if (op == SW)
        {
            //don't have WB cycle
            inst_cnt_run++;
            inst_cnt_left--;
        }
        else if (op == BEQ)
        { //don't have WB cycle
            inst_cnt_run++;
            inst_cnt_left--;
        }
    }

} //end of WB

void Controller::panic()
{

    fprintf(stderr, "Simulator Panic!\n");
    exit(-1);
}

void Controller::DumpMemory(WordMemory mem)
{
    cout << "\nInst MEMORY DUMP: " << std::endl;
    for (auto i = 0; i < 2048; i += 4)
    {
        cout << std::dec << i << ":0x" << std::hex << mem.get(i) << "\t\t\t";
        if ((i + 1) % 8 == 0)
        {
            cout << std::endl;
        }
    }

    cout << std::endl;
}

void Controller::dump_IFID()
{

    Instruction pinst(IFID.getIR());
    pinst.init();
    cout << "Latch IFID:\n";
    cout << "\tIR: " << pinst._inst_hex_str << " " << pinst._inst_bin_str << " " << pinst._inst_asm_str << "\n";
    cout << "\tNPC: " << IFID.getNPC() << "\n";
}

void Controller::dump_IDEXE()
{
    Instruction pinst(IDEX.getIR());
    pinst.init();
    cout << "Latch IDEXE:\n";
    cout << "\tIR: " << pinst._inst_hex_str << " " << pinst._inst_bin_str << " " << pinst._inst_asm_str << "\n";
    cout << "\tNPC: " << IDEX.getNPC() << "\n";
    cout << "\tA: " << IDEX.getA() << "\n";
    cout << "\tB: " << IDEX.getB() << "\n";
    cout << "\tImm: " << IDEX.getImm() << "\n";
}

void Controller::dump_EXMEM()
{
    Instruction pinst(EXMEM.getIR());
    pinst.init();
    cout << "Latch EXMEM:\n";
    cout << "\tIR: " << pinst._inst_hex_str << " " << pinst._inst_bin_str << " " << pinst._inst_asm_str << "\n";
    cout << "\tB: " << EXMEM.getB() << "\n";
    cout << "\tALUOutput: " << EXMEM.getALUOutput() << "\n";
    cout << "\tCond: " << EXMEM.getCond() << "\n";
}

void Controller::dump_MEMWB()
{
    Instruction pinst(MEMWB.getIR());
    pinst.init();
    cout << "Latch MEMWB:\n";
    cout << "\tIR: " << pinst._inst_hex_str << " " << pinst._inst_bin_str << " " << pinst._inst_asm_str << "\n";
    cout << "\tALUOutput: " << MEMWB.getALUOutput() << "\n";
    cout << "\tLMD: " << MEMWB.getLMD() << "\n";
}

bool Controller::peep_data_hazard(uint32_t inst)
{

    Instruction pinst(inst);
    pinst.init();

    if (verbose)
    {
        cout << stage << clock << " :peep hazard, inst at IF " << pinst._inst_asm_str << endl;
    }

    bool rst = false;

    if (pinst.is_nop())
    {
        return rst;
    }

    auto rs = pinst._rs;
    auto rt = pinst._rt;
    auto rd = pinst._rd;
    auto op = pinst._op;
    auto type = pinst._type;
    auto funct = pinst._funct;

    switch (type)

    {
    case R_TYPE:
        if ((funct == ADD) ||
            (funct == AND) ||
            (funct == OR) ||
            (funct == SUB))
        {
            /** HAzard detection */
            if ((R_to_be_write[rs] > 0) || (R_to_be_write[rt] > 0))
            {
                rst = true;
            }
            break;
        }
        else if ((funct == SLL) || (funct == SRL))
        {

            /** HAzard detection */
            if (R_to_be_write[rt] > 0)
            {
                rst = true;
            }
            break;
        }
        else if (funct == MUL)
        {

            if ((R_to_be_write[rs] > 0) || (R_to_be_write[rt] > 0))
            {
                rst = true;
            }
            break;
        }

    case I_TYPE:
        if ((op == ADDI) ||
            (op == ANDI) ||
            (op == ORI) ||
            (op == SLTI) ||
            (op == SLTIU))
        {

            if (R_to_be_write[rs] > 0)
            {
                rst = true;
            }
            break;
        }
        else if (op == LUI)
        {
            break;
        }
        else if (op == LW)
        {

            if (R_to_be_write[rs] > 0)
            {
                rst = true;
            }
            break;
        }
        else if (op == SW)
        {
            if (R_to_be_write[rs] > 0 || R_to_be_write[rt] > 0)
            {
                rst = true;
            }
            break;
        }
        else if (op == BEQ)
        {
            //branch = true;
            //hazard_ctrl_cnt++; //detect here is to late.  Moved to the IF stage.

            /* data hazard  */
            if (R_to_be_write[rs] > 0 || R_to_be_write[rt] > 0)
            {
                rst = true;
            }
            break;
        }

    /* no j_type in the project 3 */
    case J_TYPE:
        panic();
    default:
        panic();
    }

    return rst;
}

void Controller::_lock_reg(uint32_t inst)
{
    Instruction pinst(inst);
    pinst.init();

    if (pinst.is_nop())
    {
        return;
    }

    auto rs = pinst._rs;
    auto rt = pinst._rt;
    auto rd = pinst._rd;
    auto op = pinst._op;
    auto type = pinst._type;
    auto funct = pinst._funct;

    switch (type)

    {
    case R_TYPE:
        if ((funct == ADD) ||
            (funct == AND) ||
            (funct == OR) ||
            (funct == SUB))
        {
            R_to_be_write[rd]++;
            break;
        }
        else if ((funct == SLL) || (funct == SRL))
        {
            R_to_be_write[rd]++;
            break;
        }
        else if (funct == MUL)
        {

            R_to_be_write[rd]++;
            R_to_be_write[rd + 1]++;

            break;
        }

    case I_TYPE:
        if ((op == ADDI) ||
            (op == ANDI) ||
            (op == ORI) ||
            (op == SLTI) ||
            (op == SLTIU))
        {
            R_to_be_write[rt]++;
            break;
        }
        else if (op == LUI)
        {
            R_to_be_write[rt]++;
            break;
        }
        else if (op == LW)
        {

            R_to_be_write[rt]++;

            break;
        }
        else if (op == SW)
        {
        }
        else if (op == BEQ)
        {
            //branch = true;
        }

        break;
    }
}

void Controller::_unlock_reg(uint32_t inst)
{

    if (clock == 3)
    {
        cout << "";
    }

    Instruction pinst(inst);
    pinst.init();
    if (pinst.is_nop())
    {
        return;
    }

    auto rs = pinst._rs;
    auto rt = pinst._rt;
    auto rd = pinst._rd;
    auto op = pinst._op;
    auto type = pinst._type;
    auto funct = pinst._funct;
    /*clear the data hazard */
    switch (type)

    {
    case R_TYPE:
        if ((funct == ADD) ||
            (funct == AND) ||
            (funct == OR) ||
            (funct == SUB))
        {
            R_to_be_write[rd]--;

            if (R_to_be_write[rd] < 0)
            {
                R_to_be_write[rd] = 0;
            }
            break;
        }
        else if ((funct == SLL) || (funct == SRL))
        {
            R_to_be_write[rd]--;
            if (R_to_be_write[rd] < 0)
            {
                R_to_be_write[rd] = 0;
            }
            break;
        }
        else if (funct == MUL)
        {

            R_to_be_write[rd]--;
            R_to_be_write[rd + 1]--;

            if (R_to_be_write[rd] < 0)
            {
                R_to_be_write[rd] = 0;
            }

            if (R_to_be_write[rd + 1] < 0)
            {
                R_to_be_write[rd + 1] = 0;
            }
            break;
        }

    case I_TYPE:
        if ((op == ADDI) ||
            (op == ANDI) ||
            (op == ORI) ||
            (op == SLTI) ||
            (op == SLTIU))
        {

            R_to_be_write[rt]--;

            if (R_to_be_write[rt] < 0)
            {
                R_to_be_write[rt] = 0;
            }

            break;
        }
        else if (op == LUI)
        {
            R_to_be_write[rt]--;

            if (R_to_be_write[rt] < 0)
            {
                R_to_be_write[rt] = 0;
            }
            break;
        }
        else if (op == LW)
        {
            R_to_be_write[rt]--;

            if (R_to_be_write[rt] < 0)
            {
                R_to_be_write[rt] = 0;
            }
            break;
        }
        else if (op == SW)
        {

            break;
        }
        else if (op == BEQ)
        {
            break;
        }

    case J_TYPE:
        panic();
    default:
        panic();
    }

    if (hazard_data_raw_cnt < 0)
    {
        hazard_data_raw_cnt = 0;
    }

    if (hazard_ctrl_cnt < 0)
    {
        hazard_ctrl_cnt = 0;
    }
}

void Controller::statistic()
{
    float cc = clock - 1;
    //utilization of each cycle
    cout << "STATISTIC:\n";
    cout << "\tClock Cycle: " << std::dec << clock - 1 << std::endl;
    cout << "\tIF Utilization: " << IF_hit << ", " << std::dec << (float)100 * (float)IF_hit / cc << "%" << std::endl;
    cout << "\tID Utilization: " << ID_hit << ", " << std::dec << (float)100 * (float)ID_hit / cc << "%" << std::endl;
    cout << "\tEX Utilization: " << EX_hit << ", " << std::dec << (float)100 * (float)EX_hit / cc << "%" << std::endl;
    cout << "\tMEM Utilization: " << MEM_hit << ", " << std::dec << (float)100 * (float)MEM_hit / cc << "%" << std::endl;
    cout << "\tWB Utilization: " << WB_hit << ", " << std::dec << (float)100 * (float)WB_hit / cc << "%" << std::endl;

    //total time in cycle taken to execute the instructions
}

uint64_t Controller::ALU(uint32_t inst)
{
    auto inst_ex = IDEX.getIR();
    Instruction pinst_ex(inst_ex);
    pinst_ex.init();

    if (clock == 15)
    {
        cout << "";
    }

    if (pinst_ex.is_nop())
    {
        panic(); //should never be called by nop
    }
    auto h = pinst_ex._shamt;
    auto itype = pinst_ex._type;

    unsigned long long int a = IDEX.getA();
    unsigned long long int b = IDEX.getB();
    uint32_t imm = IDEX.getImm();
    int32_t npc = IDEX.getNPC();

    auto op = pinst_ex._op;
    auto func = pinst_ex._funct;

    unsigned long long int out;
    /* ALU INST */
    if (itype == R_TYPE)
    {
        switch (func)

        {
        /* ALU_Register_Register */
        case ADD:
            out = a + b;
            break;
        case AND:
            out = a & b;
            break;
        case MUL:
            out = a * b;
            break;
        case OR:
            out = a | b;
            break;
        case SLL:
            out = b << h;
            break;
        case SRL:
            out = b >> h;
            break;
        case SUB:
            out = a - b;
            break;
        default:
            panic();
        }
    }
    else if (itype == I_TYPE)
    {
        switch (op)
        {
        /*ALU Register_Immediate */
        case ADDI:
            out = a + (int64_t)imm;
            break;
        case ANDI:
            out = a & imm;
            break;
        case LUI:
            out = imm << 16;
            break;
        case ORI:
            out = a | imm;
            break;
        case SLTI:
            out = (int64_t)a < (int64_t)imm ? 1 : 0;
            break;
        case SLTIU:
            out = (uint64_t)a < (uint64_t)imm ? 1 : 0;
            break;
        case LW:
            out = a + imm;
            EXMEM_n.setB(IDEX.getB());

            if (out > 2044)
            {
                fprintf(stderr, "Invalid Memory Access! vadd %ld > 2044\n", out);
                panic();
            }
            break;
        case SW:
            out = a + imm;
            EXMEM_n.setB(IDEX.getB());

            if (out > 2044)
            {
                fprintf(stderr, "Invalid Memory Access! vadd %ld > 2044\n", out);
                panic();
            }

            break;

        case BEQ:
            out = npc + (imm << 2);
            inst_cnt_left -= imm;
            inst_cnt_skip += imm;
            //cout << "BEQ skip " << imm << " instructions." << endl;

            if (a == b)
            {
                EXMEM_n.setCond(1);
                hazard_ctrl_cnt--;
                //cout << "EX" << clock << ":beq set EXMEM condition " << endl;
                //cout << "EX" << clock << ":out  " << out << endl;

                if (out > 2044)
                {
                    fprintf(stderr, "Invalid Memory Access! vadd %ld > 2044\n", out);
                    panic();
                }
            }

            break;
        }
    }

    return out;
}
