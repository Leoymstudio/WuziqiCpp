#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <windows.h>

class CConsole {
public:
    // 设置光标位置 (x, y)
    static void SetCursorPos(int x, int y);
    
    // 隐藏光标 (让界面更美观)
    static void HideCursor();
    
    // 设置颜色 (0-15)
    static void SetColor(int colorID);
};

#endif