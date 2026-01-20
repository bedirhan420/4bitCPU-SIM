#ifndef INSTRUCTIONSET_H
#define INSTRUCTIONSET_H

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdint>

namespace ISA { 

    inline bool IS_TURKISH = false; 

    // MNEMONIC LOOKUP TABLES
    inline const std::string MNEMONICS_EN[] = {
        "NOP", "LDA", "LDI", "STA", "ADD", "SUB", "AND", "OR",
        "XOR", "LDAI", "STAI", "JMP", "JZ", "JC", "CALL", "EXT"
    };

    inline const std::string MNEMONICS_TR[] = {
        "BOS", "YUK", "SAB", "SAK", "TOP", "CIK", "VE", "VEY",
        "YAD", "DOL", "SDK", "GIT", "SIF", "ELD", "CAG", "EK"
    };

    // Extended (0xF) 
    inline const std::string SUB_EN[] = { "HLT", "RST", "OUT", "NOT", "PUSH", "POP", "RET" };
    inline const std::string SUB_TR[] = { "DUR", "BAS", "YAZ", "DEG", "IT",   "CEK", "DON" };

    // MNEMONIC GETTER 
    inline std::string getMnemonic(uint8_t opcode, uint8_t operand) {
        // 1.Lang Selection
        const std::string* TABLE_MAIN = IS_TURKISH ? MNEMONICS_TR : MNEMONICS_EN;
        const std::string* TABLE_SUB  = IS_TURKISH ? SUB_TR : SUB_EN;

        // 2. Extended Instruction Control
        if (opcode == 0xF) {
            if (operand < 7) return TABLE_SUB[operand];
            return "???";
        }

        // 3. Normal Instruction
        if (opcode < 16) return TABLE_MAIN[opcode];
        return "ERR";
    }

    // OPCODES 
    inline const std::map<std::string, uint8_t> OPCODES = {
        // EN
        {"NOP",0x0}, {"LDA",0x1}, {"LDI",0x2}, {"STA",0x3},
        {"ADD",0x4}, {"SUB",0x5}, {"AND",0x6}, {"OR", 0x7},
        {"XOR",0x8}, {"LDAI",0x9}, {"STAI",0xA}, {"JMP",0xB}, 
        {"JZ", 0xC}, {"JC", 0xD},  {"CALL",0xE},
        // TR
        {"BOS",0x0}, {"YUK",0x1}, {"SAB",0x2}, {"SAK",0x3},
        {"TOP",0x4}, {"CIK",0x5}, {"VE", 0x6}, {"VEY",0x7},
        {"YAD",0x8}, {"DOL",0x9}, {"SDK",0xA}, {"GIT",0xB}, 
        {"SIF",0xC}, {"ELD",0xD}, {"CAG",0xE}
    };

    // Extended Parser 
    inline const std::map<std::string, uint8_t> EXTENDED_PARSER = {
        {"HLT",0x0}, {"RST",0x1}, {"OUT",0x2}, {"NOT",0x3},
        {"PUSH",0x4},{"POP",0x5}, {"RET",0x6},
        {"DUR",0x0}, {"BAS",0x1}, {"YAZ",0x2}, {"DEG",0x3},
        {"IT", 0x4}, {"CEK",0x5}, {"DON",0x6}
    };

   inline bool isTwoByteInstruction(int opcode) { // JMP, JZ, JC, CALL
        return (opcode == 0xB || opcode == 0xC || opcode == 0xD || opcode == 0xE);
    }
    
    inline bool isTwoByteInstruction(const std::string& mnemonic) {
        if (OPCODES.find(mnemonic) == OPCODES.end()) return false;
        return isTwoByteInstruction((int)OPCODES.at(mnemonic));
    }

    inline bool isExtendedInstruction(const std::string& m, uint8_t& subCode) {
        if (EXTENDED_PARSER.count(m)) {
            subCode = EXTENDED_PARSER.at(m);
            return true;
        }
        return false;
    }
}

#endif