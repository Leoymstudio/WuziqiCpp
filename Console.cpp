#include "Console.h"
#include <iostream>

void CConsole::SetCursorPos(int x, int y) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    // x * 2 因为中文字符（棋盘符号）在控制台占2个单位宽度
    COORD pos = { (SHORT)(x * 2), (SHORT)y }; 
    SetConsoleCursorPosition(hOut, pos);
}

void CConsole::HideCursor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

void CConsole::SetColor(int colorID) {
    // 0=黑, 7=白, 12=红(高亮), 8=灰
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorID);
}