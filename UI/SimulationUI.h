#ifndef SIMULATION_UI_H
#define SIMULATION_UI_H

#include "raylib.h"
#include "../Core/CPU.h"
#include "../Core/Peripherals.h"
#include "../Utils/Utils.h"
#include "../Utils/Constants.h"

void DrawRegisters(const CPU4bit& cpu) {
    DrawRectangle(50, 120, 260, 200, COLOR_SIDEBAR);
    DrawRectangleLines(50, 120, 260, 200, GRAY);
    
    DrawText("ACC:", 60, 130, 20, WHITE); 
    DrawText(TextFormat("%d (0x%X)", cpu.ACC, cpu.ACC), 120, 130, 20, COLOR_ACCENT);
    
    DrawText("PC :", 60, 160, 20, WHITE); 
    DrawText(TextFormat("%03d", cpu.PC), 120, 160, 20, COLOR_ACCENT);
    
    DrawText("SP :", 60, 190, 20, WHITE); 
    DrawText(TextFormat("%d", cpu.SP), 120, 190, 20, COLOR_ACCENT);
    
    DrawText("FLAGS:", 60, 230, 10, GRAY);
    DrawText("Z", 120, 230, 20, cpu.Z ? RED : DARKGRAY);
    DrawText("C", 150, 230, 20, cpu.C ? YELLOW : DARKGRAY);
    
    if(cpu.halted) DrawText("HALTED", 180, 230, 20, RED);
}

void DrawRAM(const CPU4bit& cpu) {
    int startX = 350; int startY = 120;
    DrawText("RAM (DATA)", startX, 100, 20, LIGHTGRAY);
    for(int i=0; i<16; i++) {
        int x = startX + (i%4)*70;
        int y = startY + (i/4)*70;
        Color col = (i==14)?BLUE:(i==15?MAGENTA:COLOR_EDITOR);
        
        DrawRectangle(x, y, 60, 60, col);
        DrawRectangleLines(x, y, 60, 60, GRAY);

        DrawText(TextFormat("%X", cpu.RAM[i]), x+20, y+20, 20, WHITE);
        DrawText(TextFormat("[%X]", i), x+2, y+45, 10, LIGHTGRAY);
    }
}

void DrawROM(const CPU4bit& cpu) {
    int romX = 700; int romY = 120;
    DrawRectangle(romX, romY, 450, 400, COLOR_SIDEBAR);
    DrawRectangleLines(romX, romY, 450, 400, GRAY);
    DrawText("ROM (PROGRAM)", romX, 100, 20, LIGHTGRAY);
    
    int startLine = (cpu.PC > 8) ? cpu.PC - 8 : 0;
    for(int i=0; i<18; i++) {
        int addr = startLine + i;
        if(addr >= cpu.ROM.size()) break;
        int ly = romY + 30 + i*20;
        
        if(addr == cpu.PC) {
            DrawRectangle(romX+2, ly-2, 446, 18, Fade(YELLOW, 0.3f));
            DrawText(">", romX+5, ly, 10, YELLOW);
        }
        DrawText(TextFormat("%03d:", addr), romX+20, ly, 10, GRAY);
        DrawText(TextFormat("%02X", cpu.ROM[addr]), romX+55, ly, 10, DARKGRAY);
        
        // Disassemble
        bool isAddr = false;
        if(addr>0) {
            uint8_t prev = (cpu.ROM[addr-1]&0xF0)>>4;
            if(ISA::isTwoByteInstruction(prev)) isAddr=true;
        }
        std::string disasm = isAddr ? ("-> (Val: "+std::to_string(cpu.ROM[addr])+")") : Disassemble(cpu.ROM[addr]);
        DrawText(disasm.c_str(), romX+90, ly, 10, isAddr?SKYBLUE:WHITE);
    }
}

void DrawOutputPanel(CPU4bit& cpu) {
    int panelX = 50; int panelY = 550;
    int panelW = 1100; int panelH = 100;
    
    DrawRectangle(panelX, panelY, panelW, panelH, COLOR_SIDEBAR);
    DrawRectangleLines(panelX, panelY, panelW, panelH, GRAY);
    
    DrawText("OUTPUT PORT: ", panelX + 20, panelY + 40, 20, COLOR_TEXT_G);

    uint8_t leds = cpu.getGPIO().getLEDs();
    
    for (int i = 3; i >= 0; i--) {
        int x = panelX + 450 + (3-i) * 100;
        int y = panelY + 50;
        bool isLit = (leds >> i) & 1;

        if(isLit) 
            DrawCircleGradient(x, y, 20, RED, MAROON);
        else 
            DrawCircle(x, y, 20, COLOR_OFF_LED); 
        
        DrawCircleLines(x, y, 20, GRAY);
        DrawText(isLit ? "1" : "0", x-4, y-10, 20, WHITE);
        DrawText(TextFormat("LED %d", i), x-15, y+30, 10, GRAY);
    }
}

void DrawInputPopup(CPU4bit& cpu) {
    if (!cpu.isWaitingForInput) return; 

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

    int boxW = 500; int boxH = 300;
    int boxX = (GetScreenWidth() - boxW) / 2;
    int boxY = (GetScreenHeight() - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, COLOR_POPUP_BG);
    DrawRectangleLines(boxX, boxY, boxW, boxH, ORANGE);

    DrawText("INPUT REQUESTED (LDA 14)", boxX + 120, boxY + 20, 20, ORANGE);
    DrawText("Set Switches & Press Send", boxX + 140, boxY + 50, 10, LIGHTGRAY);

    GPIO_Unit& gpio = cpu.getGPIO();
    uint8_t switches = gpio.getSwitches();

    for (int i = 3; i >= 0; i--) {
        int swX = boxX + 100 + (3-i) * 80;
        int swY = boxY + 100;
        bool isOn = (switches >> i) & 1;

        Rectangle swRect = { (float)swX, (float)swY, 40, 60 };
        
        if (CheckCollisionPointRec(GetMousePosition(), swRect)) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                gpio.ToggleSwitch(i); 
            }
            DrawRectangleLinesEx(swRect, 2, WHITE);
        }

        DrawRectangleRec(swRect, GRAY);
        if (isOn) {
            DrawRectangle(swX+2, swY+2, 36, 28, GREEN);
            DrawText("1", swX+15, swY+5, 20, BLACK);
        } else {
            DrawRectangle(swX+2, swY+30, 36, 28, RED);
            DrawText("0", swX+15, swY+35, 20, WHITE);
        }
    }

    Rectangle btnRect = { (float)boxX + 150, (float)boxY + 220, 200, 50 };
    bool btnHover = CheckCollisionPointRec(GetMousePosition(), btnRect);

    bool enterPressed = IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_KP_ENTER);
    
    DrawRectangleRec(btnRect, (btnHover || enterPressed) ? COLOR_BTN_H : COLOR_BTN); 
    DrawText("SEND DATA", boxX + 190, boxY + 235, 20, WHITE);

    if ((btnHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        cpu.ResolveInput(gpio.getSwitches());
    }
}

#endif