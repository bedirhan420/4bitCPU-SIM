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

struct CompileResult {
    bool success;
    std::string errorMessage;
    int errorLineIndex; 
    Executable exe;
};


class Assembler{
private:
    std::map<std::string, int> symbolTable;
    int dataRAMOffset = 0;

    std::string Trim(const std::string& str){
        size_t first = str.find_first_not_of(" \t\r\n"); // first char is not gap from left to right
        if (std::string::npos == first) return ""; // empty string
        size_t last = str.find_last_not_of(" \t\r\n"); // first char is not gap from right to left
        return str.substr(first,(last-first+1)); // start , how many
    }

    std::string RemoveComments(const std::string& line){
        size_t commentPos = line.find(";");
        if (commentPos != std::string::npos) return line.substr(0,commentPos);
        return line;
    }

public:
    Assembler(){
    }

    CompileResult Assemble(const std::string& sourceCode){
        CompileResult result;
        result.success = true;
        result.errorLineIndex = -1;

        std::vector<std::string> codeLines;
        std::vector<int> originalLineIndices;

        std::stringstream ss(sourceCode);
        std::string line;
        int currentLineIdx = 0;
        
        bool inDataSection = false;
        symbolTable.clear();
        dataRAMOffset = 0;
        
        while (std::getline(ss,line))
        {
            std::string rawLine = line;
            line = RemoveComments(line);
            line = Trim(line);
            if (line.empty()) {currentLineIdx++; continue;}

            if (line == ".data") { inDataSection = true; currentLineIdx++; continue; }
            if (line == ".code") { inDataSection = false; currentLineIdx++; continue; }

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
                    ls.seekg(0);ls.clear();
                }
                
                int val;
                while (ls >> val)
                {
                    result.exe.initialRAM[dataRAMOffset] = (uint8_t)val;
                    dataRAMOffset++;
                }
            }else{
                codeLines.push_back(line);
                originalLineIndices.push_back(currentLineIdx);
            }
            currentLineIdx++;
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
                symbolTable[Trim(temp.substr(0,col))] = addr; // save the text before the : as a label.
                temp = Trim(temp.substr(col+1)); //continue processing the remaining part.
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
        for (size_t i = 0; i < codeLines.size(); ++i) {   
            // instruct
            std::string temp = codeLines[i];
            if (temp.back() == ':') continue;
            size_t col = temp.find(':');
            if (col != std::string::npos) temp = Trim(temp.substr(col+1)); // clip label , take instruct
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
                            try{ 
                                operand = std::stoi(cleanStr);
                            }catch(...)
                            {
                                result.success = false;
                                result.errorMessage = "Invalid Operand: " + opStr;
                                result.errorLineIndex = originalLineIndices[i];
                                return result;
                            };      
                        }                                      
                    }
                }   
            }else{
                result.success = false;
                result.errorMessage = "Unknown Instruction: " + m;
                result.errorLineIndex = originalLineIndices[i];
                return result;
            }


            if (ISA::isTwoByteInstruction(opcode)) // such as JMP 32 => Byte 1 (JMP<<4) : [1011 0000] , Byte 2 (32) : [0010 0000] => 0xB0 0x20
            {
                result.exe.machineCode.push_back((opcode << 4));
                result.exe.machineCode.push_back(operand);
            }else // such as  ADD 5 => Nibble 1 (ADD): [0100]  , Nibble 2 (5): [0101] : [0100 0101] => 0x45
            {
                result.exe.machineCode.push_back((opcode << 4)|(operand & 0xF));
            }
        }
        return result;
    }

};


#endif