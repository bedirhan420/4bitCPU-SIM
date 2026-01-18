#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <iostream>
#include <vector>
#include <string> 
#include <sstream> // string stream
#include <map> // for hashtable ds
#include <algorithm> // for find , sort etc.

#include "InstructionSet.h" 

struct Executable
{
    std::vector<uint8_t> machineCode;
    std::map<int,uint8_t> initialRAM; 
};


class Assembler{
private:
    std::map<std::string, int> symbolTable;
    int dataRAMOffset = 0;

    std::string trim(const std::string& str){
        size_t first = str.find_first_not_of(" \t\r\n"); // first char is not gap from left to right
        if (std::string::npos == first) return ""; // empty string
        size_t last = str.find_last_not_of(" \t\r\n"); // first char is not gap from right to left
        return str.substr(first,(last-first+1)); // start , how many
    }

    std::string removeComments(const std::string& line){
        size_t commentPos = line.find(";");
        if (commentPos != std::string::npos) return line.substr(0,commentPos);
        return line;
    }

public:
    Assembler(){
    }

    Executable assemble(const std::string& sourceCode){
        Executable exe;
        std::vector<std::string> codeLines;
        std::stringstream ss(sourceCode);
        std::string line;
        
        bool inDataSection = false;
        bool inCodeSection = false;

        symbolTable.clear();
        dataRAMOffset = 0;
        
        while (std::getline(ss,line))
        {
            line = removeComments(line);
            line = trim(line);
            if (line.empty()) continue;

            if (line==".data") {inDataSection = true;inCodeSection=false;continue;}
            if (line==".code") {inDataSection = false;inCodeSection=true;continue;}

            if (inDataSection)
            {
                std::stringstream ls(line);
                std::string token;
                ls >> token;

                if (token.back() == ':')
                {
                    std::string label = token.substr(0,token.length()-1);
                    symbolTable[label] = dataRAMOffset;
                }else
                {
                    ls.seekg(0);
                    ls.clear();
                }
                
                int val;
                while (ls >> val)
                {
                    exe.initialRAM[dataRAMOffset] = (uint8_t)val;
                    dataRAMOffset++;
                }
            }else if (inCodeSection || !inDataSection)
            {
                codeLines.push_back(line);
            }
        }
        
        // First Pass => Symbol Resolution , label
        int addr = 0; // virtual PC
        for (const auto& l : codeLines)
        {
            std::string temp = l;
            // Just label such as "LOOP:"
            if (temp.back() == ':'){
                symbolTable[temp.substr(0,temp.length()-1)] = addr;
                continue; // label doesn't take up space in tag memory; 
            }
            size_t col = temp.find(':');
            // both label and instruct such as "START: MOV A, 5"
            if (col != std::string::npos)
            {
                symbolTable[trim(temp.substr(0,col))] = addr; // save the text before the : as a label.
                temp = trim(temp.substr(col+1)); //continue processing the remaining part.
            }
            std::stringstream ls(temp);
            std::string m ; ls >> m; // Take the first word (Mnemonic) ;  >> : read until whitespace
            std::transform(m.begin(),m.end(),m.begin(),::toupper); // how much memory does this instruction occupy
            
            // if not in map => wrong or extended
            int opcodeID = 0;
            if (ISA::OPCODES.count(m)) opcodeID = ISA::OPCODES.at(m);
            else opcodeID = 0;
            
            addr += (ISA::isTwoByteInstruction(m)?2:1);
        }

        // Second Pass => Machine Code Generation
        for (const auto& l : codeLines)
        {   
            // instruct
            std::string temp = l;
            if (temp.back() == ':') continue;
            size_t col = temp.find(':');
            if (col != std::string::npos) temp = trim(temp.substr(col+1)); // clip label , take instruct
            if (temp.empty()) continue;

            // opcode 
            std::stringstream ls(temp);
            std::string m,opStr;
            ls >> m; // take opcode such as LDA
            std::transform(m.begin(),m.end(),m.begin(),::toupper);
         
            uint8_t opcode = 0;
            uint8_t operand = 0;
            uint8_t subCode = 0;
            
            // EXTENDED
            if (ISA::isExtendedInstruction(m, subCode)) {
                opcode = 0xF;    
                operand = subCode; 
            }
            else if (ISA::OPCODES.count(m)) {
                opcode = ISA::OPCODES.at(m);
                if (ls >> opStr) // second word (operand) such as "5" , "[10]" , "LOOP"
                {
                    if (symbolTable.count(opStr)) operand = symbolTable[opStr]; // take label address from symbol table
                    else // number or address
                    {   
                        std::string cleanStr = opStr;
                        cleanStr.erase(std::remove(cleanStr.begin(),cleanStr.end(),'['),cleanStr.end());
                        cleanStr.erase(std::remove(cleanStr.begin(),cleanStr.end(),']'),cleanStr.end()); // remove [ ] for address
                        
                        if (symbolTable.count(cleanStr))
                        {
                            operand = symbolTable[cleanStr];
                        }else
                        {
                            try{ operand = std::stoi(cleanStr);}catch(...){operand=0;}; // string to int        
                        }                                      
                    }
                }   
            }else{
                std::cerr << "Error: Unknowned Instruction -> " << m << "\n";
                return {};
            }


            if (ISA::isTwoByteInstruction(opcode)) // such as JMP 32 => Byte 1 (JMP<<4) : [1011 0000] , Byte 2 (32) : [0010 0000] => 0xB0 0x20
            {
                exe.machineCode.push_back((opcode << 4));
                exe.machineCode.push_back(operand);
            }else // such as  ADD 5 => Nibble 1 (ADD): [0100]  , Nibble 2 (5): [0101] : [0100 0101] => 0x45
            {
                exe.machineCode.push_back((opcode << 4)|(operand & 0xF));
            }
        }
        return exe;
    }

};


#endif