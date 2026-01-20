#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include "raylib.h"
#include <vector>
#include <string>
#include <sstream>
#include <algorithm> 
#include "../Utils/Constants.h"
#include "../Core/InstructionSet.h"

struct TextPos{
    int line;int col;
    bool operator == (const TextPos& other) const {return line==other.line && col==other.col;}
    bool operator!=(const TextPos& other) const { return !(*this == other); }
    bool operator<(const TextPos& other) const {
        if (line != other.line) return line < other.line;
        return col < other.col;
    }
    bool operator>(const TextPos& other) const { return other < *this; }
};

struct EditorState {
    std::vector<std::string> lines;
    TextPos cursor;
    TextPos selectionStart;
    TextPos selectionEnd;
};

class TextEditor{
private:
    std::vector<EditorState> undoStack;
    std::vector<EditorState> redoStack;
    const size_t MAX_HISTORY = 1000;
public:
    std::vector<std::string> lines;

    TextPos cursor = {0,0};
    TextPos selectionStart = {-1,-1};
    TextPos selectionEnd = {-1,-1};
    bool isSelecting = false;

    float scrollOffsetY = 0.0f;

    int errorLine = -1;
    
    Font editorFont;      
    bool fontLoaded = false;
    float fontSize = 20.0f;
    float charSpacing = 0.0f;
    int charWidth = 12;  
    
    double lastClickTime = 0;
    TextPos lastClickPos = {-1,-1};

    bool showSearch = false;         
    std::string searchQuery = "";    
    std::vector<TextPos> searchResults; 
    int currentMatchIndex = -1;

    TextEditor() {
        editorFont = GetFontDefault();
    }

    ~TextEditor(){
        SaveFile("autosave.asm",GetFullText());
        TraceLog(LOG_INFO, "Autosave successful: autosave.asm");
    }

    void UpdateSearchResults() {
        searchResults.clear();
        currentMatchIndex = -1;

        if (searchQuery.empty()) return;

        for (int i = 0; i < lines.size(); i++) {
            size_t pos = lines[i].find(searchQuery, 0);
            while(pos != std::string::npos) {
                searchResults.push_back({i, (int)pos});
                pos = lines[i].find(searchQuery, pos + 1);
            }
        }

        if (!searchResults.empty()) {
            currentMatchIndex = 0;
            GoToMatch(0);
        }
    }

    void GoToMatch(int index) {
        if (index < 0 || index >= searchResults.size()) return;
        
        currentMatchIndex = index;
        TextPos match = searchResults[index];
        
        cursor = match;
        selectionStart = match;
        selectionEnd = {match.line, match.col + (int)searchQuery.length()};

        int currentY = match.line * (int)fontSize;
        if (currentY < scrollOffsetY) scrollOffsetY = currentY;
        else if (currentY > scrollOffsetY + 500) scrollOffsetY = currentY - 300;
    }

    void NextMatch() {
        if (searchResults.empty()) return;
        int next = currentMatchIndex + 1;
        if (next >= searchResults.size()) next = 0; 
        GoToMatch(next);
    }

    void ToggleSearch() {
        showSearch = !showSearch;
        if (showSearch) {
            searchQuery = ""; 
            searchResults.clear();
            ResetSelection();
        } else {
            ResetSelection();
        }
    }

    void SaveState() {
        EditorState state;
        state.lines = lines;
        state.cursor = cursor;
        state.selectionStart = selectionStart;
        state.selectionEnd = selectionEnd;

        undoStack.push_back(state);

        if (undoStack.size() > MAX_HISTORY) {
            undoStack.erase(undoStack.begin());
        }

        redoStack.clear();
    }

    void Undo() {
        if (undoStack.empty()) return;

        EditorState currentState;
        currentState.lines = lines;
        currentState.cursor = cursor;
        currentState.selectionStart = selectionStart;
        currentState.selectionEnd = selectionEnd;
        redoStack.push_back(currentState);

        EditorState prevState = undoStack.back();
        undoStack.pop_back();

        lines = prevState.lines;
        cursor = prevState.cursor;
        selectionStart = prevState.selectionStart;
        selectionEnd = prevState.selectionEnd;
    }

    void Redo() {
        if (redoStack.empty()) return;

        EditorState currentState;
        currentState.lines = lines;
        currentState.cursor = cursor;
        currentState.selectionStart = selectionStart;
        currentState.selectionEnd = selectionEnd;
        undoStack.push_back(currentState);

        EditorState nextState = redoStack.back();
        redoStack.pop_back();

        lines = nextState.lines;
        cursor = nextState.cursor;
        selectionStart = nextState.selectionStart;
        selectionEnd = nextState.selectionEnd;
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
        if (content.empty()){ lines.push_back("");return;}
        
        std::stringstream ss(content);
        std::string segment;
        while (std::getline(ss,segment)){ 
            if (!segment.empty() && segment.back() == '\r') segment.pop_back();
            lines.push_back(segment);
        }
        if (lines.empty()) lines.push_back("");
        cursor = {0,0};
        errorLine = -1;
    }

    std::string GetFullText(){
        std::string fullCode = "";
        for (const auto& l : lines) fullCode += l + "\n";
        return fullCode;
    }

    void ResetSelection(){
        selectionStart = {-1, -1};
        selectionEnd = {-1, -1};
        isSelecting = false;
    }

    bool HasSelection() {
        return selectionStart != selectionEnd && selectionStart.line != -1;
    }

    void GetNormalizedSelection(TextPos& start, TextPos& end) {
        if (selectionStart < selectionEnd) {
            start = selectionStart; end = selectionEnd;
        } else {
            start = selectionEnd; end = selectionStart;
        }
    }

    void DeleteSelection() {
        if (!HasSelection()) return;

        TextPos start, end;
        GetNormalizedSelection(start, end);

        if (start.line == end.line) {
            lines[start.line].erase(start.col, end.col - start.col);
        } else {
            lines[start.line].erase(start.col); 
            lines[end.line].erase(0, end.col);  
            lines[start.line] += lines[end.line]; 
            
            lines.erase(lines.begin() + start.line + 1, lines.begin() + end.line + 1);
        }

        cursor = start;
        ResetSelection();
    }

    void CopyToClipboard(){
        if (!HasSelection()) return;
        TextPos start , end;
        GetNormalizedSelection(start,end);

        std::string copiedText = "";
        if (start.line == end.line){
            copiedText = lines[start.line].substr(start.col,end.col-start.col);
        }else{
            copiedText += lines[start.line].substr(start.col) + '\n';
            for (int i = start.line +1; i < end.line; i++)
            {
                copiedText += lines[i] + '\n';
            }
            copiedText += lines[end.line].substr(0,end.col);
        }
        SetClipboardText(copiedText.c_str());
        
    }

    void PasteFromClipboard(){
        if (HasSelection()) DeleteSelection();

        const char* text = GetClipboardText();
        if (text == nullptr) return;

        std::string str(text);
        std::stringstream ss(text);
        std::string segment;

        std::vector<std::string> newLines;
        while (std::getline(ss,segment))
        {
            if (!segment.empty() && segment.back() == '\r') segment.pop_back();
            newLines.push_back(segment);
        }

        if (newLines.empty()) return;

        std::string remainder = lines[cursor.line].substr(cursor.col);
        lines[cursor.line] = lines[cursor.line].substr(0,cursor.col) + newLines[0];

        for (size_t i = 1; i < newLines.size(); i++)
        {
            lines.insert(lines.begin()+cursor.line + i,newLines[i]);
        }

        int lastIndex = cursor.line + newLines.size()-1;
        lines[lastIndex] += remainder;

        cursor.line = lastIndex;
        cursor.col = (newLines.size() == 1) ? (cursor.col + newLines[0].length()) : (newLines.back().length());
    }

    void SelectWordAt(TextPos pos){
        std::string& line = lines[pos.line];
        if (line.empty()) return;

        int start = pos.col;
        int end = pos.col;

        while (start > 0 && isalnum(line[start-1])) start --;
        while (end < line.length() && isalnum(line[end])) end++;

        selectionStart = {pos.line,start};
        selectionEnd = {pos.line,end};
        cursor = selectionEnd;
    }

    void HandleInput(){
        Vector2 mouse = GetMousePosition();
        Rectangle editorBounds = {20, 60, 1160, 600};

        bool isMouseOver = CheckCollisionPointRec(mouse,editorBounds);
        SetMouseCursor(isMouseOver ? MOUSE_CURSOR_IBEAM:MOUSE_CURSOR_DEFAULT);

        bool ctrlPressed = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL) || IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER);
        bool shiftPressed = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);

        if (ctrlPressed && IsKeyPressed(KEY_F)) {
            ToggleSearch();
            return;
        }

        if (showSearch) {
            int key = GetKeyPressed();
            while(key > 0) {
                if (key == KEY_ESCAPE) {
                    ToggleSearch();
                } else if (key == KEY_ENTER) {
                    NextMatch();
                } else if (key == KEY_BACKSPACE) {
                    if (searchQuery.length() > 0) {
                        searchQuery.pop_back();
                        UpdateSearchResults();
                    }
                }
                key = GetKeyPressed();
            }

            int charCode = GetCharPressed();
            while (charCode > 0) {
                if (charCode >= 32 && charCode <= 125) {
                    searchQuery += (char)charCode;
                    UpdateSearchResults();
                }
                charCode = GetCharPressed();
            }
            return; 
        }

        if (ctrlPressed) {
            if (IsKeyPressed(KEY_Z)) {
                if (shiftPressed) Redo(); // Ctrl + Shift + Z -> Redo
                else Undo();              // Ctrl + Z -> Undo
                return;
            }
            if (IsKeyPressed(KEY_Y)) { // Ctrl + Y -> Redo (
                Redo();
                return;
            }
        }
        // Mouse
        if (isMouseOver)
        {   
            // Scroll
            float wheel = GetMouseWheelMove();
            if (wheel != 0) {
                scrollOffsetY -= wheel * fontSize * 3; 
                if (scrollOffsetY < 0) scrollOffsetY = 0;
            }
            // Click
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                int clickedLine = (mouse.y - 70 + scrollOffsetY) / (int)fontSize;
                if (clickedLine < 0) clickedLine = 0;
                if (clickedLine >= lines.size()) clickedLine = lines.size() - 1;

                int relativeX = mouse.x - 60;
                int clickedCol = (relativeX + (charWidth / 2)) / charWidth;
                if (clickedCol < 0) clickedCol = 0;
                if (clickedCol > lines[clickedLine].length()) clickedCol = lines[clickedLine].length();

                TextPos newPos = {clickedLine, clickedCol};

                double currTime = GetTime();
                if (currTime- lastClickTime < 0.5 && newPos == lastClickPos){
                    SelectWordAt(newPos);
                }else{
                    if (shiftPressed){
                        if (!HasSelection()) selectionStart = cursor;
                        selectionEnd = newPos;
                    }else{
                        ResetSelection();
                        selectionStart = newPos; 
                        cursor = newPos;
                    }
                }
                lastClickTime = currTime;
                lastClickPos = newPos;
            }
            // Drag
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                int currentLine = (mouse.y - 70 + scrollOffsetY) / (int)fontSize;
                if (currentLine < 0) currentLine = 0;
                if (currentLine >= lines.size()) currentLine = lines.size() - 1;

                int relativeX = mouse.x - 60;
                int currentCol = (relativeX + (charWidth / 2)) / charWidth;
                if (currentCol < 0) currentCol = 0;
                if (currentCol > lines[currentLine].length()) currentCol = lines[currentLine].length();

                selectionEnd = {currentLine, currentCol};
                cursor = selectionEnd;
            }
        }
                
        if (ctrlPressed) {
            if (IsKeyPressed(KEY_C)) CopyToClipboard();
            if (IsKeyPressed(KEY_V)) {SaveState();PasteFromClipboard();}
            if (IsKeyPressed(KEY_X)) { SaveState();CopyToClipboard(); DeleteSelection(); }
            if (IsKeyPressed(KEY_A)) { // Select All
                selectionStart = {0, 0};
                selectionEnd = {(int)lines.size() - 1, (int)lines.back().length()};
                cursor = selectionEnd;
            }
            return; 
        }

        int key = GetKeyPressed();
        while (key > 0) {
            if (key == KEY_ENTER || key == KEY_TAB || key == KEY_BACKSPACE) {
                SaveState();
            }
            if (key == KEY_ENTER) {
                if (HasSelection()) DeleteSelection();
                lines.insert(lines.begin() + cursor.line + 1, lines[cursor.line].substr(cursor.col));
                lines[cursor.line] = lines[cursor.line].substr(0, cursor.col);
                cursor.line++; cursor.col = 0;
            } 
            else if (key == KEY_TAB) {
                if (HasSelection()) DeleteSelection();
                lines[cursor.line].insert(cursor.col, "    ");
                cursor.col += 4;
            } 
            else if (key == KEY_BACKSPACE) {
                if (HasSelection()) {
                    DeleteSelection();
                } else {
                    if (cursor.col > 0) {
                        lines[cursor.line].erase(cursor.col - 1, 1); cursor.col--;
                    } else if (cursor.line > 0) {
                        int prevLen = lines[cursor.line - 1].length();
                        lines[cursor.line - 1] += lines[cursor.line];
                        lines.erase(lines.begin() + cursor.line);
                        cursor.line--; cursor.col = prevLen;
                    }
                }
            }
            else if (key == KEY_UP) { 
                if (cursor.line > 0) cursor.line--; 
                ResetSelection();
            }
            else if (key == KEY_DOWN) { 
                if (cursor.line < lines.size() - 1) cursor.line++; 
                ResetSelection();
            }
            else if (key == KEY_LEFT) { 
                if (cursor.col > 0) cursor.col--; 
                ResetSelection();
            }
            else if (key == KEY_RIGHT) { 
                if (cursor.col < lines[cursor.line].length()) cursor.col++; 
                ResetSelection();
            }

            if (cursor.col > lines[cursor.line].length()) cursor.col = lines[cursor.line].length();
            
            // Scroll Track
            int visibleLines = 600 / (int)fontSize;
            int currentY = cursor.line * (int)fontSize;
            
            if (currentY < scrollOffsetY) {
                scrollOffsetY = currentY;
            } else if (currentY > scrollOffsetY + 580) { 
                scrollOffsetY = currentY - 580;
            }

            key = GetKeyPressed();
        }
        
        int charCode = GetCharPressed();
        while (charCode > 0) {
            if ((charCode >= 32) && (charCode <= 125)) {
                SaveState();
                if (HasSelection()) DeleteSelection();
                lines[cursor.line].insert(cursor.col, 1, (char)charCode);
                cursor.col++;
            }
            charCode = GetCharPressed();
        }
    }

    Color GetTokenColor(std::string token) {
        if (token.empty()) return COLOR_TEXT_NORMAL;
        if (token[0] == '.') return COLOR_DIRECTIVE;
        if (token.back() == ':') return COLOR_LABEL;
        if (isdigit(token[0])) return COLOR_NUMBER;

        std::string upperToken = token;
        std::transform(upperToken.begin(), upperToken.end(), upperToken.begin(), ::toupper);

        if (ISA::OPCODES.count(upperToken)) return COLOR_INSTRUCTION;
        if (ISA::EXTENDED_PARSER.count(upperToken)) return COLOR_INSTRUCTION;

        return COLOR_OPERAND; 
    }

    void Draw(){
        DrawRectangle(20,60,1160,600,COLOR_EDITOR);
        DrawRectangleLines(20,60,1160,600,GRAY);
        BeginScissorMode(22, 62, 1156, 596);

        TextPos selStart = {-1, -1}, selEnd = {-1, -1};
        if (HasSelection()) GetNormalizedSelection(selStart, selEnd);

        int startY = 70;
        int startX = 60;

        // Visible lines
        int startLineIndex = (int)(scrollOffsetY / fontSize);
        int endLineIndex = startLineIndex + (600 / (int)fontSize) + 2;
        if (startLineIndex < 0) startLineIndex = 0;
        if (endLineIndex > lines.size()) endLineIndex = lines.size();

        for (size_t i = startLineIndex; i < endLineIndex; ++i)
        {
            int posY = startY + (i * (int)fontSize) - (int)scrollOffsetY;

            if ((int)i == errorLine) DrawRectangle(22,posY,1156, (int)fontSize, COLOR_ERROR);
            
            if ((int)i == cursor.line && !HasSelection()){
                DrawRectangle(22, posY, 1156, (int)fontSize, Fade(WHITE, 0.05f));
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

            for (size_t j = 0; j < line.length(); ++j)
            {
                int charX = startX + (j*charWidth);

                bool isSearchMatch = false;
                bool isCurrentMatch = false;
                if (!searchResults.empty()) {
                    for (int k = 0; k < searchResults.size(); k++) {
                        if (searchResults[k].line == i) {
                            if (j >= searchResults[k].col && j < searchResults[k].col + searchQuery.length()) {
                                isSearchMatch = true;
                                if (k == currentMatchIndex) isCurrentMatch = true;
                                break;
                            }
                        }
                    }
                }

                if (isSearchMatch) {
                    DrawRectangle(charX, posY, charWidth, (int)fontSize, isCurrentMatch ? ORANGE : YELLOW);
                    charColors[j] = BLACK; 
                }

                if (HasSelection())
                {
                    TextPos currPos = {(int)i,(int)j};
                    bool inRange = false;
                    if (i > selStart.line && i < selEnd.line) inRange = true; 
                    else if (i == selStart.line && i == selEnd.line) {
                        if (j >= selStart.col && j < selEnd.col) inRange = true;
                    } 
                    else if (i == selStart.line && j >= selStart.col) inRange = true;
                    else if (i == selEnd.line && j < selEnd.col) inRange = true; 

                    if (inRange) {
                        DrawRectangle(charX, posY, charWidth, (int)fontSize, Fade(BLUE, 0.4f));
                    }
                }
                char str[2] = {line[j], '\0'};
                DrawTextEx(editorFont, str, {(float)charX, (float)posY}, fontSize, charSpacing, charColors[j]);
            }
            if ((int)i == cursor.line && !HasSelection()) {
                if ((int)(GetTime() * 2) % 2 == 0) {
                    int cursorX = startX + (cursor.col * charWidth);
                    DrawRectangle(cursorX, posY, 2, (int)fontSize, GREEN);
                }
            }
        }
        EndScissorMode();
        // Scroll Bar
        if (lines.size() * fontSize > 600) {
             float scrollPerc = scrollOffsetY / (lines.size() * fontSize);
             float barHeight = 100; 
             DrawRectangle(1170, 60 + (500 * scrollPerc), 8, barHeight, Fade(GRAY, 0.5f));
        }

        if (showSearch) {
            int boxW = 300; int boxH = 40;
            int boxX = 1160 - boxW - 10;
            int boxY = 65;
            
            DrawRectangle(boxX, boxY, boxW, boxH, DARKGRAY);
            DrawRectangleLines(boxX, boxY, boxW, boxH, WHITE);
            
            DrawText("FIND:", boxX + 10, boxY + 12, 20, LIGHTGRAY);
            DrawText(searchQuery.c_str(), boxX + 70, boxY + 12, 20, WHITE);
            
            if ((int)(GetTime()*2)%2==0) {
                int textW = MeasureText(searchQuery.c_str(), 20);
                DrawRectangle(boxX + 70 + textW, boxY + 10, 2, 20, WHITE);
            }

            if (!searchResults.empty()) {
                std::string countStr = std::to_string(currentMatchIndex + 1) + "/" + std::to_string(searchResults.size());
                DrawText(countStr.c_str(), boxX + boxW - 60, boxY + 12, 10, YELLOW);
            } else if (!searchQuery.empty()) {
                DrawText("0/0", boxX + boxW - 40, boxY + 12, 10, RED);
            }
        }
    }
};

#endif