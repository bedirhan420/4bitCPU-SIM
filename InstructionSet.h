#ifndef INSTRUCTIONSET_H
#define INSTRUCTIONSET_H

#include <string>
#include <map>
#include <vector>
#include <algorithm>

namespace ISA { 

    // (Inline for link error)
    inline const std::map<std::string, uint8_t> OPCODES = {
        {"NOP",0x0}, {"LDA",0x1}, {"LDI",0x2}, {"STA",0x3},
        {"ADD",0x4}, {"SUB",0x5}, {"AND",0x6}, {"OR", 0x7},
        {"XOR",0x8}, {"NOT",0x9}, {"OUT",0xA}, {"JMP",0xB},
        {"JZ", 0xC}, {"JC", 0xD}, {"RST",0xE}, {"HLT",0xF}
    };

    inline std::string getMnemonic(uint8_t opcode) {
        for (const auto& pair : OPCODES) {
            if (pair.second == opcode) {
                return pair.first;
            }
        }
        return "???"; 
    }

    inline bool isJumpInstruction(uint8_t opcode) { // JMP, JZ, JC
        return (opcode == 0xB || opcode == 0xC || opcode == 0xD);
    }

    inline bool isNotOperandInstruction(uint8_t opcode) { // NOP, OUT, RST, HLT, NOT 
        return (opcode == 0x0 || opcode == 0xA || opcode == 0xE || opcode == 0xF || opcode == 0x9);
    }
    
    inline bool isJumpInstruction(const std::string& mnemonic) {
        if (OPCODES.find(mnemonic) == OPCODES.end()) return false;
        return isJumpInstruction(OPCODES.at(mnemonic));
    }
}

#endif