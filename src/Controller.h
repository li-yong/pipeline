#ifndef controller_h
#define controller_h

#include "WordMemory.h"
#include "latchReg.h"

typedef uint32_t inst_len;

class Controller
{
public:
    Controller();
    ~Controller();

    void init(WordMemory, std::string, int, int, bool, bool);

    void run_inst_mode();

    void run_cycle_mode();

    //bool ended();

    bool verbose;
    bool interactive;

    //private:
    //PC pc{};

    /*latch has been flushed. Present value. */
    uint32_t pc;
    uint32_t ppc;

    latchReg IFID;
    latchReg IDEX;
    latchReg EXMEM;
    latchReg MEMWB;

    /*latches have not been flushed. Next value. */
    latchReg IFID_n;
    latchReg IDEX_n;
    latchReg EXMEM_n;
    latchReg MEMWB_n;

    WordMemory data_memory;
    WordMemory inst_memory;

    uint64_t clock = 0; //number of cycle
    uint64_t utilized = 0;
    std::string stage;
    std::string instr_asm;

    //bool data_stall; // stall due to data hazard
    //bool ctl_stall;  // stall due to control hazard

    std::string mode;  //cycle or instuction mode
    int number_to_run; //hom many cycle or inst to run
    int inst_cnt_all;  //number of inst in the inst_memory
    int inst_cnt_run;  //number of inst has been run
    int inst_cnt_skip;
    int inst_cnt_left;

    uint32_t IF_hit;
    uint32_t ID_hit;
    uint32_t EX_hit;
    uint32_t MEM_hit;
    uint32_t WB_hit;

    int R_to_be_read[32];  //whether the registers will be read. 0 false, >0 true.
    int R_to_be_write[32]; //whether the registers will be write

    int insert_nop;

    int hazard_data_raw_cnt;
    int hazard_data_waw_cnt;
    int hazard_data_war_cnt;
    int hazard_ctrl_cnt;
    int branch;

    int inst_fetched_cnt;

    void IF();
    void ID();
    void EX();
    void MEM();
    void WB();

    uint64_t clock_cycles();

    void panic();
    uint32_t run();

    //oid InspectLatches();

    void DumpMemory(WordMemory mem);

    //void inspect_and_wait();

    void cycle_utilized();

    void dump_IFID();
    void dump_IDEXE();
    void dump_EXMEM();
    void dump_MEMWB();

    void dump_all();

    bool peep_data_hazard(uint32_t inst);

    void _lock_reg(uint32_t inst);
    void _unlock_reg(uint32_t inst);

    void statistic();
    uint64_t ALU(uint32_t inst);
    uint64_t set_branch_addr(uint32_t inst);

    int ask_number_of_run(std::string);
    void ask_dump();
    int ask_dump_option();

private:
};

#endif