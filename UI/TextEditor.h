#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include "raylib.h"
#include <vector>
#include <string>
#include <sstream>
#include <algorithm> 
#include "../Utils/Constants.h"
#include "../Core/InstructionSet.h"

class TextEditor{
public:
    std::vector<std::string> lines;
    int cursorLine = 0;
    int cursorCol  = 0;
    int errorLine = -1;
    
    Font editorFont;      
    bool fontLoaded = false;
    float fontSize = 20.0f;
    float charSpacing = 0.0f;
    int charWidth = 12;   

    TextEditor() {
        editorFont = GetFontDefault();
    }

    void SetFont(Font font, float size) {
        editorFont = font;
        fontSize = size;
        fontLoaded = true;

        Vector2 sizeVec = MeasureTextEx(editorFont, "M", fontSize, charSpacing);
        charWidth = (int)sizeVec.x; 
    }

    void LoadText(const std::string& content){
        lines.clear();
        std::stringstream ss(content);
        std::string segment;
        while (std::getline(ss,segment)) lines.push_back(segment);
        if (lines.empty()) lines.push_back("");
        cursorLine = 0; cursorCol  = 0; errorLine = -1;
    }

    std::string GetFullText(){
        std::string fullCode = "";
        for (const auto& l : lines) fullCode += l + "\n";
        return fullCode;
    }

    Color GetTokenColor(std::string token) {
        if (token.empty()) return COLOR_TEXT_NORMAL;
        if (token[0] == '.') return COLOR_DIRECTIVE;
        if (token.back() == ':') return COLOR_LABEL;
        if (isdigit(token[0])) return COLOR_NUMBER;

        std::string upperToken = token;
        std::transform(upperToken.begin(), upperToken.end(), upperToken.begin(), ::toupper);

        if (ISA::OPCODES.find(upperToken) != ISA::OPCODES.end()) return COLOR_INSTRUCTION;
        for (const auto& pair : ISA::SUBCODES) {
            if (pair.second == upperToken) return COLOR_INSTRUCTION;
        }
        return COLOR_OPERAND; 
    }

    void HandleInput(){
        Vector2 mouse = GetMousePosition();
        Rectangle editorBounds = {20, 60, 1160, 600};

        if (CheckCollisionPointRec(mouse, editorBounds)) SetMouseCursor(MOUSE_CURSOR_IBEAM);
        else SetMouseCursor(MOUSE_CURSOR_DEFAULT);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, editorBounds)) {
            int clickedLine = (mouse.y - 70) / (int)fontSize; 
            if (clickedLine < 0) clickedLine = 0;
            if (clickedLine >= lines.size()) clickedLine = lines.size() - 1;
            cursorLine = clickedLine;

            int relativeX = mouse.x - 60;
            int clickedCol = (relativeX + (charWidth / 2)) / charWidth;

            if (clickedCol < 0) clickedCol = 0;
            if (clickedCol > lines[cursorLine].length()) clickedCol = lines[cursorLine].length();
            cursorCol = clickedCol;
        }

        int key = GetKeyPressed();
        while (key > 0) {
            if (key == KEY_ENTER) {
                lines.insert(lines.begin() + cursorLine + 1, "");
                cursorLine++; cursorCol = 0;
            } else if (key == KEY_TAB) {
                lines[cursorLine].insert(cursorCol, "    ");
                cursorCol += 4;
            } else if (key == KEY_BACKSPACE) {
                if (cursorCol > 0) {
                    lines[cursorLine].erase(cursorCol - 1, 1); cursorCol--;
                } else if (cursorLine > 0) {
                    int prevLen = lines[cursorLine-1].length();
                    lines[cursorLine-1] += lines[cursorLine];
                    lines.erase(lines.begin() + cursorLine);
                    cursorLine--; cursorCol = prevLen;
                }
            }
            else if (key == KEY_UP) { if(cursorLine > 0) cursorLine--; }
            else if (key == KEY_DOWN) { if(cursorLine < lines.size()-1) cursorLine++; }
            else if (key == KEY_LEFT) { if(cursorCol > 0) cursorCol--; }
            else if (key == KEY_RIGHT) { if(cursorCol < lines[cursorLine].length()) cursorCol++; }
            
            if (cursorCol > lines[cursorLine].length()) cursorCol = lines[cursorLine].length();
            key = GetKeyPressed();
        }
        
        int charCode = GetCharPressed();
        while (charCode > 0) {
            if ((charCode >= 32) && (charCode <= 125)) {
                lines[cursorLine].insert(cursorCol, 1, (char)charCode);
                cursorCol++;
            }
            charCode = GetCharPressed();
        }
    }

    void Draw(){
        DrawRectangle(20,60,1160,600,COLOR_EDITOR);
        DrawRectangleLines(20,60,1160,600,GRAY);

        for (size_t i = 0; i < lines.size(); ++i)
        {
            int posY = 70 + (i * (int)fontSize); 
            int startX = 60; 

            if ((int)i == errorLine) DrawRectangle(22,posY,1156, (int)fontSize, COLOR_ERROR);
            
            if ((int)i == cursorLine){
                DrawRectangle(22, posY, 1156, (int)fontSize, Fade(WHITE, 0.05f));
                
                int cursorX = startX + (cursorCol * charWidth);
                if ((int)(GetTime()*2)%2==0) {
                    DrawRectangle(cursorX, posY, 2, (int)fontSize, GREEN); 
                }
            }

            DrawTextEx(editorFont, TextFormat("%2d", i+1), {(float)30, (float)posY}, fontSize, charSpacing, GRAY);

            std::string line = lines[i];
            std::vector<Color> charColors(line.length(), COLOR_TEXT_NORMAL);
            
            std::string currentWord = "";
            int wordStartIndex = 0;

            for (size_t j = 0; j <= line.length(); ++j) {
                bool isSplitter = (j == line.length()) || (line[j] == ' ') || (line[j] == ',') || (line[j] == ';');
                
                if (isSplitter) {
                    if (!currentWord.empty()) {
                        Color col = GetTokenColor(currentWord);
                        for(size_t k=0; k<currentWord.length(); k++) charColors[wordStartIndex + k] = col;
                        currentWord = "";
                    }
                    if (j < line.length() && line[j] == ';') {
                         for(size_t k=j; k<line.length(); k++) charColors[k] = COLOR_COMMENT;
                         break; 
                    }
                    wordStartIndex = j + 1;
                } else {
                    currentWord += line[j];
                }
            }

            for (size_t j = 0; j < line.length(); ++j) {
                char str[2] = { line[j], '\0' };
                int charX = startX + (j * charWidth);
                
                DrawTextEx(editorFont, str, {(float)charX, (float)posY}, fontSize, charSpacing, charColors[j]);
            }
        }
    }
};

#endif