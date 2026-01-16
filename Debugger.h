#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <bitset>
#include <cstdlib>

#include "CPU.h"
#include "InstructionSet.h"

void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

std::string disassemble(uint8_t byte) {
    uint8_t opcode = (byte & 0xF0) >> 4;
    uint8_t operand = (byte & 0x0F);
    
    std::string opStr = ISA::getMnemonic(opcode);
    if (opStr.length() == 2) opStr += " ";

    if (ISA::isJumpInstruction(opcode)) {
        return opStr + " (Jump)"; 
    } else if (ISA::isNotOperandInstruction(opcode)) {
        return opStr; 
    } else {
        return opStr + " [" + std::to_string(operand) + "]";
    }
}

void drawUI(const CPU4bit& cpu) {
    clearScreen();
    std::cout << "=== 4-BIT CPU EMULATOR (Harvard Arch) ===\n";
    // REGISTERS
    std::cout << "| PC : " << std::setw(3) << (int)cpu.PC 
              << " | ACC: " << std::setw(2) << std::hex << (int)cpu.ACC 
              << " (" << std::bitset<4>(cpu.ACC) << ") |" 
              << " FLAGS: [ Z:" << cpu.Z << " C:" << cpu.C << " ] |\n";
    std::cout << "-----------------------------------------\n";
    //RAM
    std::cout << "| RAM (Memory Mapped I/O):              |\n";
    for (int i = 0; i < 16; i++) {
        if (i == 8) std::cout << "\n";
        if (i % 8 == 0) std::cout << "| ";
        
        std::cout << "[" << std::hex << i << "]:" 
                  << std::setw(1) << (int)cpu.RAM[i];

        if (i == 14) std::cout << "(IN) ";
        else if (i == 15) std::cout << "(OUT)";
        else std::cout << "     ";
    }
    std::cout << "\n-----------------------------------------\n";

    // ROM 
    std::cout << "| ROM (Code Window):                    |\n";
    
    int startLine = (cpu.PC > 2) ? cpu.PC - 2 : 0; 
    int endLine = startLine + 6; 

    for (int i = startLine; i < endLine; ++i) {
        if (i >= 256) break; 

        std::cout << "| ";
        if (i == cpu.PC) std::cout << ">> ";
        else             std::cout << "   ";

        std::cout << std::setw(3) << std::setfill('0') << std::dec << i << ": " 
                  << std::hex << std::uppercase << std::setw(2) << (int)cpu.ROM[i] 
                  << "  ";

        bool isAddressData = false;
        if (i > 0) {
            uint8_t prevOp = (cpu.ROM[i-1] & 0xF0) >> 4;
            if (prevOp >= 0xB && prevOp <= 0xD) isAddressData = true;
        }

        if (isAddressData) {
            std::cout << "-> (Address: " << (int)cpu.ROM[i] << ")";
        } else {
            std::cout << disassemble(cpu.ROM[i]);
        }
        
        std::cout << std::setfill(' ') << "\n";
    }
    std::cout << "-----------------------------------------\n";
    std::cout << "[ENTER] STEP | [Q] QUIT\n";
}

#endif