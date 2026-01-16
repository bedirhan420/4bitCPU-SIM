#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip> // input output manipulators : console formatting 
#include <bitset> // binary visualizing
#include <ctime> // time and clock

#include "CPU.h"
#include "Assembler.h"
#include "Debugger.h"

std::string loadFile(const std::string& fileName){
    std::ifstream file(fileName);
    if (!file.is_open()) return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]){
    std::srand(std::time(0));

    std::string fileName = "program.asm";
    if (argc > 1) fileName = argv[1];

    std::string source = loadFile(fileName);
    if (source.empty()) {
        std::cout << "ERROR: " << fileName << " is not found. Please create an .asm file.\n";
        return 1;
    }

    Assembler assembler;
    CPU4bit cpu;

    cpu.setRAM(2,8);
    cpu.setRAM(3,1);
    cpu.setRAM(10, 1);

    std::vector<uint8_t> program = assembler.assemble(source);
    cpu.loadProgram(program);

    char cmd = 0;
    while (!cpu.isHalted() && cmd != 'q')
    {
        drawUI(cpu);
        cmd = std::cin.get();
        cpu.fetch();
        cpu.execute();
    }

    drawUI(cpu);
    std::cout << "\nProgram is ended.\n";
    return 0;

}