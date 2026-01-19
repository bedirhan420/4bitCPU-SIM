#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>
#include <sstream>
#include "raylib.h"
#include "../Core/InstructionSet.h"
#include "../Utils/Constants.h"
#include "../Vendor/tinyfiledialogs.h"

std::string LoadFile(const std::string& fileName){
    std::ifstream file(fileName);
    if (!file.is_open()) return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

inline void SaveFile(const std::string& fileName, const std::string& content) {
    std::ofstream file(fileName);
    file << content;
    file.close();
}

inline std::string OpenFileDialog() {
    const char* filters[1] = { "*.asm" }; // Sadece .asm dosyalari
    const char* path = tinyfd_openFileDialog(
        "Select ASM File",  // Header
        "",                 // Default Folder ('' = last folder)
        1,                  // Filter Count
        filters,            // Filters
        "Assembly Code",    // Describe
        0                   // Multiselection (0 = no)
    );

    if (path != NULL) return std::string(path);
    return ""; 
}

inline std::string SaveFileDialog() {
    const char* filters[1] = { "*.asm" };
    const char* path = tinyfd_saveFileDialog(
        "Save ASM File", 
        "program.asm",      
        1, 
        filters, 
        "Assembly Code"
    );

    if (path != NULL) return std::string(path);
    return "";
}

inline bool DrawButton(Rectangle rect, const char* text){
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse,rect);
    bool clicked = false;

    if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) clicked = true;

    DrawRectangleRec(rect,hovered ? COLOR_BTN_H : COLOR_BTN);
    DrawRectangleLinesEx(rect,1,GRAY);
    int textW = MeasureText(text,20);
    DrawText(text, rect.x + (rect.width - textW) /2 ,rect.y + 10 ,20,WHITE);

    return clicked;
}

inline std::string Disassemble(uint8_t byte) {
    uint8_t opcode = (byte & 0xF0) >> 4;
    uint8_t operand = (byte & 0x0F);
    std::string opStr = ISA::getMnemonic(opcode, operand);
    if (opStr.length() == 3) opStr += " "; 
    if (ISA::isTwoByteInstruction(opcode)) return opStr + " (Addr)"; 
    else if (opcode == 0xF || opcode == 0x0) return opStr; 
    else if (opcode == 0x2) return opStr + " " + std::to_string(operand); // LDA value
    else return opStr + " [" + std::to_string(operand) + "]";
}
#endif