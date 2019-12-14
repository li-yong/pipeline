#include <cstdint>
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <regex>
#include <stdio.h>
#include <unistd.h>

//#include <cstring>
#include <typeinfo>

#include "WordMemory.h"
#include "latchReg.h"
#include "Instruction.h"
#include "Controller.h"
#include "util.h"
//#include <fstream>

using namespace std;

void usage()
{
    cout << "Interactive Mode: main.exe -f <code.txt> [-v]\n ";
    cout << "\t -f: input instruction file" << endl;
    cout << "\t -v: verbose" << endl;

    cout << "\nNo Question Mode:\n";

    cout << "\t main.exe -f <code.txt> -m <cycle|inst>  -n <number of cycle or inst> [-v]" << endl;

    exit(-1);
}

int main(int argc, char **argv)
{
    int opt;

    string file;
    string mode; //Instruction mode or Cycle mode
    string number_s;
    bool verbose = false;
    bool interactive = false;
    int number_to_run;

    string INST = "inst";

    if (argc < 3)
    {
        usage();
    }
    else if (argc < 7)
    {
        interactive = true;

        int mode_i = 0;

        while (1)
        {
            if (mode_i != 1 && mode_i != 2)
            {
                cout << "wrong input. " << mode_i << ". valid input [1,2]\n";
                cout << "Select mode: 1 - Instruction, 2 - Cycle: ";
                cin >> mode_i;
            }
            else
            {
                break;
            }
        }

        if (mode_i == 1)
        {
            mode = "inst";
        }
        else if (mode_i == 2)
        {
            mode = "cycle";
        }
    }

    //parse for none-interactive cmd line.
    while ((opt = getopt(argc, argv, "f:m:n:v")) != -1)
    {
        switch (opt)
        {

        case 'm':
            mode = optarg;
            break;
        case 'f':
            file = optarg;
            break;
        case 'n':
            number_s = optarg;
            number_to_run = atoi(optarg);
            break;
        case 'v':
            verbose = true;
            break;
        default:
            usage();
        }
    }

    //----- load instrctions

    ifstream infile(file);

    string line;

    int address = 0;       //start address in inst memory
    WordMemory InstMemory; //instruction memory
    uint32_t ins;
    stringstream ss;
    string cmt = "#";

    int inst_cnt_all = 0;

    while (getline(infile, line))
    {
        if ((cmt.compare(line.substr(0, 1)) == 0) || (line.length() < 4))
            continue;

        ins = to_instruction(line);
        InstMemory.set(address, ins);
        inst_cnt_all++;
        address += 4;
    }
    cout << "Read input completed, total " << inst_cnt_all << " instructions"
         << " loaded." << endl;

    //---- Controller
    Controller pCtr;
    pCtr.init(InstMemory, mode, number_to_run, inst_cnt_all, verbose, interactive);
    pCtr.run();

    return (0);
}