#include "raylib.h"

#include "Utils/Constants.h"
#include "Utils/Utils.h"
#include "UI/TextEditor.h"
#include "UI/SimulationUI.h"
#include "Core/CPU.h"
#include "Core/Assembler.h"
Font appFont;

int main(){
    InitWindow(1200,700,"4-BIT CPU SIM");
    SetTargetFPS(60);

    appFont = GetFontDefault(); // LoadFontEx("resources/code.ttf", 32, 0, 250);
    Font codeFont = LoadFontEx("Inconsolata.ttf", 32, 0, 250);
    SetTextureFilter(codeFont.texture, TEXTURE_FILTER_POINT);
    AppState currState = STATE_EDITOR;

    Assembler asmb;
    CPU4bit cpu;
    TextEditor editor;
    editor.SetFont(codeFont, 20.0f);

    std::string currentFilePath = "";

    editor.LoadText(".data\n\n.code\n");

    std::string msg = "Ready.";
    Color msgColor = GRAY;

    bool autoRun = false;
    float runTimer = 0.0f;

    while (!WindowShouldClose())
    {     
        bool isCtrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL) || 
                      IsKeyDown(KEY_LEFT_SUPER)   || IsKeyDown(KEY_RIGHT_SUPER);

        if (currState == STATE_EDITOR)
        {
            editor.HandleInput();
        }else if (currState == STATE_SIMULATION)
        {
            if (cpu.isWaitingForInput)
            {
                int key = GetKeyPressed();
                int inputVal = -1;
                if (key >= KEY_ZERO && key <= KEY_NINE) inputVal = key - KEY_ZERO;
                else if (key >= KEY_KP_0 && key <= KEY_KP_9) inputVal = key - KEY_KP_0;
                else if (key >= KEY_A && key <= KEY_F) inputVal = 10 + (key - KEY_A);
                if (inputVal != -1) cpu.ResolveInput(inputVal);
            }else {
                if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                    if(!cpu.isHalted()) { cpu.Fetch(); cpu.Execute(); }
                    autoRun = false; 
                }
                
                if (IsKeyPressed(KEY_R)) {
                    autoRun = !autoRun;
                }

                if (IsKeyPressed(KEY_BACKSPACE)) {
                    cpu.Reset();
                    autoRun = false;
                }

                if (autoRun) {
                    runTimer += GetFrameTime();
                    if (runTimer >= 0.1f) {
                        runTimer = 0;
                        if (!cpu.isHalted()) { cpu.Fetch(); cpu.Execute(); }
                        else autoRun = false;
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(COLOR_BG);

        DrawRectangle(0, 0, 1200, 50, COLOR_SIDEBAR);
        DrawTextEx(appFont,"4-BIT IDE",(Vector2){20, 15}, 20, 1, LIGHTGRAY);

        if (DrawButton((Rectangle){150,5,100,40},"EDITOR") || IsKeyPressed(KEY_E))
        {
            currState = STATE_EDITOR;autoRun=false;
        }
        
        if (currState == STATE_EDITOR)
        {
            if (DrawButton((Rectangle){260,5,120,40},"COMPILE")){
                CompileResult res = asmb.Assemble(editor.GetFullText());
                if (res.success)
                {
                    cpu.LoadProgram(res.exe.machineCode,res.exe.initialRAM);
                    cpu.consoleBuffer = "Compilation Successful.";
                    currState = STATE_SIMULATION;
                    SaveFile("program.asm", editor.GetFullText());
                }else
                {
                    msg = "ERROR: " + res.errorMessage;
                    editor.errorLine = res.errorLineIndex;
                    msgColor = RED;
                }
            }
            if (DrawButton((Rectangle){490, 5, 100, 40}, "SAVE") || (isCtrl && IsKeyPressed(KEY_S))) {
                std::string path = SaveFileDialog();
                if (!path.empty()) {
                    currentFilePath = path;
                    SaveFile(currentFilePath, editor.GetFullText());
                    msg = "Saved New: " + currentFilePath;
                    msgColor = GREEN;
                }
            }
            if (DrawButton((Rectangle){600, 5, 80, 40}, "LOAD")|| (isCtrl && IsKeyPressed(KEY_L))) {
                std::string path = OpenFileDialog();
                if (!path.empty()) {
                    std::string content = LoadFile(path);
                    if (!content.empty()) {
                        editor.LoadText(content);
                        currentFilePath = path;
                        msg = "Loaded: " + currentFilePath;
                        msgColor = BLUE;
                    }
                }
            }
            editor.Draw();
            DrawText(msg.c_str(), 20, 670, 20, msgColor);
        }else
        {
            DrawRectangleLinesEx((Rectangle){260, 5, 120, 40}, 2, GREEN);
            if (DrawButton((Rectangle){50, 60, 80, 40}, "STEP")) {
                if(!cpu.isHalted() && !cpu.isWaitingForInput) { cpu.Fetch(); cpu.Execute(); }
                autoRun = false;
            }
            if (DrawButton((Rectangle){140, 60, 80, 40}, autoRun?"PAUSE":"RUN")) autoRun = !autoRun;
            if (DrawButton((Rectangle){230, 60, 80, 40}, "RESET")) {
                cpu.Reset();
                autoRun = false; 
            }
            DrawText("Shortcuts: [Space/Enter]: Step | [R]: Run/Stop | [<=]: Reset", 350, 70, 10, GRAY);
            DrawRegisters(cpu);
            DrawRAM(cpu);
            DrawROM(cpu);
            DrawOutputPanel(cpu); 
            DrawInputPopup(cpu);
        }
        EndDrawing();
    }
    UnloadFont(codeFont);
    CloseWindow();
    return 0;
}