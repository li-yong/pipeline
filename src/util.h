#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <regex>
#include <stdio.h>
#include <unistd.h>

using namespace std;
void stage_header(int clock, string stage);
uint32_t to_instruction(std::string &code);

#endif