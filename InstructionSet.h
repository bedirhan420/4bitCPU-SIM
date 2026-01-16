#ifndef INSTRUCTIONSET_H
#define INSTRUCTIONSET_H

#include <string>
#include <map>
#include <vector>
#include <algorithm>

namespace ISA { 

    // (Inline for link error)
    // Standard Instructs => with Operand
    inline const std::map<std::string, uint8_t> OPCODES = {
        {"NOP",0x0}, {"LDA",0x1}, {"LDI",0x2}, {"STA",0x3},
        {"ADD",0x4}, {"SUB",0x5}, {"AND",0x6}, {"OR", 0x7},
        {"XOR",0x8}, {"JMP", 0xB}, {"JZ",  0xC}, {"JC",  0xD},
        {"CALL", 0xE}
    };
    
    // Standard Codes => without Operand
    inline bool isExtendedInstruction(const std::string& m, uint8_t& subCode) {
        if (m == "HLT")  { subCode = 0x0; return true; }
        if (m == "RST")  { subCode = 0x1; return true; }
        if (m == "OUT")  { subCode = 0x2; return true; }
        if (m == "NOT")  { subCode = 0x3; return true; }
        if (m == "PUSH") { subCode = 0x4; return true; } 
        if (m == "POP")  { subCode = 0x5; return true; } 
        if (m == "RET")  { subCode = 0x6; return true; } 
        return false;
    }
    
    // Deassembler for debugger UI
    inline std::string getMnemonic(uint8_t opcode, uint8_t operand) {
        // If Opcode is F , found the insruction acording to Operand
        if (opcode == 0xF) {
            switch(operand) {
                case 0x0: return "HLT";
                case 0x1: return "RST";
                case 0x2: return "OUT";
                case 0x3: return "NOT";
                case 0x4: return "PUSH";
                case 0x5: return "POP";
                case 0x6: return "RET";
                default:  return "EXT?";
            }
        }
        
        // Standard instructions
        for (const auto& pair : OPCODES) {
            if (pair.second == opcode) return pair.first;
        }
        return "???";
    }

   inline bool isTwoByteInstruction(int opcode) { // JMP, JZ, JC, CALL
        return (opcode == 0xB || opcode == 0xC || opcode == 0xD || opcode == 0xE);
    }
    
    inline bool isTwoByteInstruction(const std::string& mnemonic) {
        if (OPCODES.find(mnemonic) == OPCODES.end()) return false;
        return isTwoByteInstruction((int)OPCODES.at(mnemonic));
    }
}

#endif