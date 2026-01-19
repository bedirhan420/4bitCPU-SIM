#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "raylib.h"

enum AppState {
    STATE_EDITOR,
    STATE_SIMULATION
};

// COLORS
#define COLOR_BG        CLITERAL(Color){ 30, 30, 30, 255 }
#define COLOR_EDITOR    CLITERAL(Color){ 40, 40, 40, 255 }
#define COLOR_SIDEBAR   CLITERAL(Color){ 25, 25, 25, 255 }
#define COLOR_ACCENT    CLITERAL(Color){ 0, 120, 215, 255 } 
#define COLOR_ERROR     CLITERAL(Color){ 200, 50, 50, 100 } 
#define COLOR_BTN       CLITERAL(Color){ 60, 60, 60, 255 }
#define COLOR_BTN_H     CLITERAL(Color){ 80, 80, 80, 255 }
#define COLOR_OFF_LED   CLITERAL(Color){ 50, 0, 0, 255 }
#define COLOR_POPUP_BG  CLITERAL(Color){40, 45, 50, 255}
#define COLOR_TEXT_W    WHITE
#define COLOR_TEXT_G    GRAY
#define COLOR_TEXT_NORMAL   WHITE
#define COLOR_INSTRUCTION   CLITERAL(Color){ 86, 156, 214, 255 }  // Blue (LDA, ADD)
#define COLOR_DIRECTIVE     CLITERAL(Color){ 197, 134, 192, 255 } // Purple (.data, .code)
#define COLOR_LABEL         CLITERAL(Color){ 220, 220, 170, 255 } // Yellow (DONGU:)
#define COLOR_NUMBER        CLITERAL(Color){ 181, 206, 168, 255 } // Green (Sayılar)
#define COLOR_COMMENT       CLITERAL(Color){ 106, 153, 85, 255 }  // Dark Green (Commands ; adad)
#define COLOR_OPERAND       CLITERAL(Color){ 156, 220, 254, 255 } // White Blue (Variable ref)

#endif