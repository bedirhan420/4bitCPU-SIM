#ifndef CPU_H
#define CPU_H

#include <vector> // Dynamic Array 
#include <cstdint> // Integers with certain sizes
#include <iostream> // input output stream
#include <cstdlib> // C standard : mem , translations , ...
#include <map>
#include <string>

class CPU4bit{
public:
    // REGISTERS
    uint8_t ACC = 0; // Accumulator Register (4 bit)
    uint8_t PC = 0; // Program Counter Register (8 bit)
    uint8_t IR = 0; // Instruction Register (8 bit)
    uint8_t SP = 0 ; // Stack Pointer Register (8 bit)
    
    //FLAGS
    bool Z = false; // Zero Flag
    bool C = false; // Carry Flag

    //MEMORY (HARVARD)
    std::vector<uint8_t> ROM; // Program Memmory (256 byte)
    std::vector<uint8_t> RAM; // Data Memmory (16 nibble)
    std::vector<uint8_t> STACK; // Stack (16 )

    bool halted = false;

    std::string consoleBuffer = "System Ready.";

    CPU4bit(){
        ROM.resize(256,0);
        RAM.resize(16,0);
        STACK.resize(16, 0);
    }

    void loadProgram(const std::vector<uint8_t>& code , const std::map<int,uint8_t>& data){
        std::fill(ROM.begin(),ROM.end(),0); 
        for (size_t i=0;i<code.size();++i){
            ROM[i] = code[i];
        }

        std::fill(RAM.begin(),RAM.end(),0);
        for (auto const& [addr,val] : data)
        {
            if (addr<RAM.size())
            {
                RAM[addr] = val & 0xF;
            }
        }
        
        PC = 0;
        SP=0;
        halted = false;
    }

    void setRAM(int addr, uint8_t val){
        if (addr >= 0 && addr<16) RAM[addr] = val & 0xF; // Lower Nibble Mask (0000 1111)
    }

    bool isHalted()const{return halted;}

    void fetch(){
        /* 
        CLK is up.
        Address Bus <- PC
        MEM_READ signal is active.
        Data Bus <- ROM[Address]
        IR <- Data Bus
        PC <- PC + 1 
        */
        if(halted)return;
        IR = ROM[PC];
        PC++;
    }

    void execute(){
        if(halted) return;

        uint8_t opcode = (IR & 0xF0) >> 4;
        uint8_t operand = (IR & 0x0F);

        switch (opcode)
        {
        case 0x0: // NOP
            break;
        case 0x1: // LDA[addr]
            if (operand == 14) {// Memmory Mapped I/O
                int val;
                std::cout << "\n>>> ENTER A VALUE (0-15): ";
                std::cin >> val;
                ACC = val & 0xF;
                RAM[14] = ACC;
            }
            else ACC = RAM[operand];
            Z = (ACC == 0);
            break;
        case 0x2: // LDI val
            ACC = operand;
            Z = (ACC == 0);
            break;
        case 0x3: // STA [addr]
            RAM[operand] = ACC;
            break;
        case 0x4: // ADD [addr]
            {
                uint16_t temp = ACC + RAM[operand];
                C = (temp > 15);
                ACC = temp & 0xF; // Masking
                Z = (ACC==0);
            }
            break;
        case 0x5: // SUB [addr]
            {
                int temp = ACC - RAM[operand];
                C = (temp<0);
                ACC = temp & 0xF;
                Z = (ACC == 0);
            }
            break;
        case 0x6: // AND [addr]
            ACC = ACC & RAM[operand];
            Z = (ACC == 0);
            break;
        case 0x7: // OR [addr]
            ACC = ACC | RAM[operand];
            Z = (ACC == 0);
            break;
        case 0x8: // XOR [addr]
            ACC = ACC ^ RAM[operand];
            Z = (ACC == 0);
            break;
        case 0x9: // LDAI [operand]
            {
                uint8_t targetAddr = RAM[operand] & 0xF;
                ACC = RAM[targetAddr];
                Z = (ACC==0);
            }
            break;
        case 0xA:
            {
                uint8_t targetAddr = RAM[operand] & 0xF;
                RAM[targetAddr] = ACC;
            }
            break;
        case 0xB: // JMP
            PC = ROM[PC];
            break;
        case 0xC: // JZ
            if(Z) PC = ROM[PC];
            else PC++;
            break;
        case 0xD: // JC
            if(C) PC = ROM[PC];
            else PC++;
            break;
        case 0xE: // CALL
            if (SP < STACK.size())
            {
                STACK[SP] = PC+1;
                SP++;
            }
            PC = ROM[PC];
            break;
        case 0xF:
            switch (operand)
            {
            case 0x0: // HLT
                halted = true;
                break;
            case 0x1: //RST
                PC = 0; ACC=0; SP=0; 
                break;
            case 0x2: // OUT
                consoleBuffer = ">>> OUTPUT: " + std::to_string((int)ACC);
                break;
            case 0x3: // NOT
                ACC = (~ACC) & 0xF;
                Z = (ACC == 0); 
                break;
            case 0x4: // PUSH
                if (SP<STACK.size()){
                    STACK[SP] = ACC;
                    SP++;
                }
                break;
            case 0x5: // POP
                if (SP>0)
                {
                    SP--;
                    ACC = STACK[SP];
                }
                break;
            case 0x6: // RET
                if (SP>0)
                {
                    SP--;
                    PC = STACK[SP];
                }
                
                break;
            }
            break;
        default:
            halted=true;
            break;
        }
    }

};

#endif