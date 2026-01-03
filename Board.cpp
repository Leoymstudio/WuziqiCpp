#include "Board.h"
#include "Console.h"
#include <iostream>
#include <iomanip> // 用于 setw

using namespace std;

CBoard::CBoard() {
    Reset();
}

void CBoard::Reset() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            m_iGrid[i][j] = EMPTY;
        }
    }
    m_stLastMove = { -1, -1 };
}

bool CBoard::IsValid(int x, int y) const {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

bool CBoard::IsEmpty(int x, int y) const {
    return IsValid(x, y) && m_iGrid[y][x] == EMPTY;
}

void CBoard::PlacePiece(int x, int y, int type) {
    if (IsValid(x, y)) {
        m_iGrid[y][x] = type;
        if (type != EMPTY) {
            m_stLastMove = { x, y };
        }
    }
}

void CBoard::UndoPiece(int x, int y) {
    if (IsValid(x, y)) {
        m_iGrid[y][x] = EMPTY;
    }
}

int CBoard::GetPiece(int x, int y) const {
    if (!IsValid(x, y)) return -1;
    return m_iGrid[y][x];
}

Point CBoard::GetLastMove() const {
    return m_stLastMove;
}

void CBoard::Draw() {
    // 这里的 Draw 不再移动光标，而是直接打印整个大方块

    // 1. 打印顶部列号 (A - O)
    cout << "   "; // 左边留空，给行号让位
    for (int i = 0; i < BOARD_SIZE; i++) {
        char c = 'A' + i;
        // 关键对齐：字母 + 空格 = 2字符宽
        // 配合下面的棋盘符号（1字符）+ 空格（1字符）= 2字符宽
        cout << c << " ";
    }
    cout << endl;

    // 2. 打印每一行
    for (int y = 0; y < BOARD_SIZE; y++) {
        // 打印行号，占2位，右对齐
        cout << setw(2) << (y + 1) << " ";

        for (int x = 0; x < BOARD_SIZE; x++) {
            int iPiece = m_iGrid[y][x];

            // 如果是最后一步，我们在字符后面加个特殊标记吗？
            // 在流式输出中，颜色可能不生效（取决于终端），但我们还是尝试设置颜色
            bool bIsLast = (x == m_stLastMove.iX && y == m_stLastMove.iY);

            // 注意：某些 Emulated Terminal 可能不支持颜色，但我们尽量保留
            if (bIsLast) CConsole::SetColor(12); // 红
            else CConsole::SetColor(7);          // 白

            if (iPiece == BLACK) {
                cout << "●";
            } else if (iPiece == WHITE) {
                cout << "○";
            } else {
                CConsole::SetColor(8); // 灰色线条
                if (y == 0) {
                    if (x == 0) cout << "┌";
                    else if (x == BOARD_SIZE - 1) cout << "┐";
                    else cout << "┬";
                } else if (y == BOARD_SIZE - 1) {
                    if (x == 0) cout << "└";
                    else if (x == BOARD_SIZE - 1) cout << "┘";
                    else cout << "┴";
                } else {
                    if (x == 0) cout << "├";
                    else if (x == BOARD_SIZE - 1) cout << "┤";
                    else cout << "┼";
                }
            }
            // 还原颜色
            CConsole::SetColor(7);

            // 关键对齐：符号后面补一个空格
            // 符号(2) + 空格(1) = 3，正好对齐上面的 " A "
            cout << " ";
        }
        cout << endl;
    }
}

// 在流式模式下，DrawNode 几乎没用了，但为了保持编译通过，留着它
void CBoard::DrawNode(int x, int y) {
    // 空实现，或者保留原样皆可，因为我们不再调用它
}