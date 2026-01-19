#include "raylib.h"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip> 

#include "CPU.h"
#include "Assembler.h"
#include "InstructionSet.h" 

#define DARK_BG    CLITERAL(Color){ 20, 20, 20, 255 }
#define PANEL_BG   CLITERAL(Color){ 30, 30, 30, 255 }
#define RAM_BOX    CLITERAL(Color){ 50, 50, 50, 255 }
#define HIGHLIGHT  CLITERAL(Color){ 200, 200, 0, 150 } 
#define PC_COLOR   CLITERAL(Color){ 0, 255, 0, 255 }   


std::string loadFile(const std::string& fileName){
    std::ifstream file(fileName);
    if (!file.is_open()) return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string disassemble(uint8_t byte) {
    uint8_t opcode = (byte & 0xF0) >> 4;
    uint8_t operand = (byte & 0x0F);
    
    std::string opStr = ISA::getMnemonic(opcode, operand);
    
    if (opStr.length() == 3) opStr += " "; 

    if (ISA::isTwoByteInstruction(opcode)) {
        return opStr + " (Addr)"; 
    } else if (opcode == 0xF || opcode == 0x0) {
        return opStr; 
    } else if (opcode == 0x2) { // LDI -> Immediate
        return opStr + " " + std::to_string(operand);
    } else {
        return opStr + " [" + std::to_string(operand) + "]";
    }
}

int main() {
    InitWindow(950, 600, "4-BIT CPU EMULATOR - GUI");
    SetTargetFPS(60); 

    Assembler asmb;
    CPU4bit cpu; 
    
    std::string sourceCode = loadFile("program.asm");
    if(sourceCode.empty()) {
        cpu.consoleBuffer = "ERROR: program.asm not found!";
    } else {
        Executable exe = asmb.assemble(sourceCode);
        if(!exe.machineCode.empty()){
            cpu.loadProgram(exe.machineCode, exe.initialRAM);
            cpu.consoleBuffer = "Program loaded successfully.";
        } else {
            cpu.consoleBuffer = "Assembler Error!";
        }
    }

    while (!WindowShouldClose()) {
        
        if (!cpu.isWaitingForInput) {
            if (IsKeyPressed(KEY_SPACE)) { 
                cpu.fetch();
                cpu.execute();
            }
        }
        else {
            int key = GetKeyPressed();
            int inputVal = -1;

            if (key >= KEY_ZERO && key <= KEY_NINE) inputVal = key - KEY_ZERO;
            else if (key >= KEY_KP_0 && key <= KEY_KP_9) inputVal = key - KEY_KP_0;
            else if (key >= KEY_A && key <= KEY_F) inputVal = 10 + (key - KEY_A);

            if (inputVal != -1) {
                cpu.resolveInput(inputVal);
            }
        }

        if (IsKeyPressed(KEY_R)) { 
            cpu.PC = 0; cpu.SP = 0;
            cpu.consoleBuffer = "System Reset.";
            sourceCode = loadFile("program.asm");
            Executable exe = asmb.assemble(sourceCode);
            cpu.loadProgram(exe.machineCode, exe.initialRAM);
        }

        BeginDrawing();
        ClearBackground(DARK_BG);

        DrawText("HARVARD ARCHITECTURE CPU", 320, 10, 20, LIGHTGRAY);
        DrawText("Controls: [SPACE] Step | [R] Reload", 340, 40, 10, GRAY);

        // --- REGISTERS ---
        DrawRectangle(30, 80, 200, 180, PANEL_BG);
        DrawRectangleLines(30, 80, 200, 180, GRAY);
        
        DrawText("REGISTERS", 40, 90, 10, ORANGE);
        DrawText(TextFormat("ACC:  %d  (0x%X)", cpu.ACC, cpu.ACC), 50, 120, 20, PC_COLOR);
        DrawText(TextFormat("PC :  %03d", cpu.PC), 50, 150, 20, WHITE);
        DrawText(TextFormat("SP :  %d", cpu.SP), 50, 180, 20, WHITE);
        
        DrawText("FLAGS:", 50, 220, 10, GRAY);
        DrawText("Z", 100, 220, 20, cpu.Z ? RED : DARKGRAY);
        DrawText("C", 130, 220, 20, cpu.C ? YELLOW : DARKGRAY);

        // --- RAM ---
        int ramX = 260;
        int ramY = 80;
        DrawText("RAM (Data Memory)", ramX, 65, 10, ORANGE);
        
        for(int i=0; i<16; i++) {
            int x = ramX + (i % 4) * 60; 
            int y = ramY + (i / 4) * 60;
            
            Color boxColor = RAM_BOX;
            if(i == 14) boxColor = BLUE;        
            else if(i == 15) boxColor = PURPLE; 
            else if(i == 3) boxColor = DARKGREEN; 
            
            DrawRectangle(x, y, 50, 50, boxColor);
            DrawRectangleLines(x, y, 50, 50, GRAY);
            DrawText(TextFormat("%X", cpu.RAM[i]), x+18, y+15, 20, WHITE);
            DrawText(TextFormat("[%X]", i), x+2, y+35, 10, LIGHTGRAY);
        }

        // --- ROM (CODE WINDOW)---
        int romX = 530;
        int romY = 80;
        DrawRectangle(romX, romY, 380, 400, PANEL_BG); 
        DrawRectangleLines(romX, romY, 380, 400, GRAY);
        DrawText("ROM (Program Memory)", romX+10, 65, 10, ORANGE);
        
        int startLine = (cpu.PC > 8) ? cpu.PC - 8 : 0;
        
        for(int i=0; i<18; i++) { 
            int addr = startLine + i;
            if (addr >= cpu.ROM.size()) break;

            int lineY = romY + 30 + (i * 20);
            
            if(addr == cpu.PC) {
                DrawRectangle(romX+2, lineY-2, 376, 18, HIGHLIGHT);
                DrawText(">", romX+5, lineY, 10, YELLOW);
            }

            // Adres
            DrawText(TextFormat("%03d:", addr), romX+20, lineY, 10, GRAY);
            // Hex Code
            DrawText(TextFormat("%02X", cpu.ROM[addr]), romX+55, lineY, 10, LIGHTGRAY);

            // --- DISASSEMBLY LOGIC---
            std::string asmText;
            bool isAddressData = false;
            
            if (addr > 0) {
                uint8_t prevOp = (cpu.ROM[addr-1] & 0xF0) >> 4;
                if (ISA::isTwoByteInstruction(prevOp)) isAddressData = true;
            }

            if (isAddressData) {
                asmText = "-> (Address: " + std::to_string((int)cpu.ROM[addr]) + ")";
                DrawText(asmText.c_str(), romX+85, lineY, 10, SKYBLUE);
            } else {
                asmText = disassemble(cpu.ROM[addr]);
                DrawText(asmText.c_str(), romX+85, lineY, 10, WHITE);
            }
        }

        // --- OUTPUT ---
        DrawRectangle(30, 500, 880, 80, BLACK);
        DrawRectangleLines(30, 500, 880, 80, DARKGRAY);
        DrawText("SYSTEM CONSOLE:", 40, 510, 10, GRAY);
        DrawText(cpu.consoleBuffer.c_str(), 40, 535, 20, YELLOW);

        if (cpu.isWaitingForInput) {
            DrawRectangle(0, 0, 950, 600, Fade(BLACK, 0.7f));

            int boxW = 400;
            int boxH = 200;
            int boxX = (950 - boxW) / 2;
            int boxY = (600 - boxH) / 2;

            DrawRectangle(boxX, boxY, boxW, boxH, PANEL_BG);
            DrawRectangleLines(boxX, boxY, boxW, boxH, ORANGE);
            
            DrawText("WAITING FOR INPUT", boxX + 80, boxY + 40, 20, ORANGE);
            DrawText("Please press a key (0-9 or A-F)", boxX + 60, boxY + 80, 20, WHITE);
            
            if ((int)(GetTime() * 2) % 2 == 0) {
                DrawText("_", boxX + 190, boxY + 120, 40, YELLOW);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}