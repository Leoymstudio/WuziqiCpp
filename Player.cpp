#include "Player.h"
#include <iostream>
#include <string>
#include <cctype> // toupper

using namespace std;

CPlayer::CPlayer(int color) : m_iColor(color) {}

CHumanPlayer::CHumanPlayer(int color) : CPlayer(color) {}

Point CHumanPlayer::MakeMove(CBoard& board) {
    while (true) {
        // 纯净的输入提示，不移动光标
        cout << endl;
        cout << ">> 玩家 " << (m_iColor == BLACK ? "黑" : "白")
             << " 落子 (如 H8): ";

        string input;
        cin >> input;

        if (input.length() < 2) continue;

        char colChar = toupper(input[0]);
        int x = -1;
        if (colChar >= 'A' && colChar <= 'O') {
            x = colChar - 'A';
        } else {
            cout << "   [错误] 列号不对 (A-O)，请重试。" << endl;
            continue;
        }

        int y = -1;
        try {
            y = stoi(input.substr(1)) - 1;
        } catch (...) {
            cout << "   [错误] 行号不对 (数字)，请重试。" << endl;
            continue;
        }

        if (board.IsValid(x, y) && board.IsEmpty(x, y)) {
            return { x, y };
        } else {
            cout << "   [错误] 该位置 (" << input << ") 无效或已有子。" << endl;
        }
    }
}