Compile:
	g++ -g --std=c++11 main.cpp  Controller.cpp  Instruction.cpp   WordMemory.cpp latchReg.cpp util.cpp -o main.exe 


Usage:
	$ ./main.exe
	Interactive Mode: main.exe -f <code.txt> [-v]
			 -f: input instruction file
			 -v: verbose

	No Question Mode:
			 main.exe -f <code.txt> -m <cycle|inst>  -n <number of cycle or inst> [-v]

Example:
	main.exe -f code.txt