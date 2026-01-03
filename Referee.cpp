#include "Referee.h"
#include <iostream>

using namespace std;

// 检查胜利
bool CReferee::CheckWin(const CBoard& board, int x, int y) {
    int color = board.GetPiece(x, y);
    if (color == EMPTY) return false;

    // 方向：横、竖、左斜、右斜
    int dx[] = { 1, 0, 1, 1 };
    int dy[] = { 0, 1, 1, -1 };

    for (int i = 0; i < 4; i++) {
        int count = 1; // 包含自己
        count += CountConsecutive(board, x, y, dx[i], dy[i], color);
        count += CountConsecutive(board, x, y, -dx[i], -dy[i], color);

        // --- 核心修正点 ---
        if (color == BLACK) {
            // 黑棋：必须严格等于5才算赢
            // 如果是6个或更多，这是长连，不算赢（稍后会被 CheckForbidden 抓获）
            if (count == 5) return true;
        } else {
            // 白棋：5个或以上都算赢
            if (count >= 5) return true;
        }
    }
    return false;
}

// 辅助：往一个方向数数
int CReferee::CountConsecutive(const CBoard& board, int x, int y, int dx, int dy, int color) {
    int count = 0;
    for (int step = 1; step < BOARD_SIZE; step++) {
        int nx = x + dx * step;
        int ny = y + dy * step;
        if (!board.IsValid(nx, ny) || board.GetPiece(nx, ny) != color) {
            break;
        }
        count++;
    }
    return count;
}

// 检查禁手 (仅限黑棋)
bool CReferee::CheckForbidden(const CBoard& board, int x, int y) {
    if (board.GetPiece(x, y) != BLACK) return false;

    int dx[] = { 1, 0, 1, 1 };
    int dy[] = { 0, 1, 1, -1 };

    int threeCount = 0;     // 活三数量
    int fourCount = 0;      // 四数量

    for (int i = 0; i < 4; i++) {
        int count = 1;
        count += CountConsecutive(board, x, y, dx[i], dy[i], BLACK);
        count += CountConsecutive(board, x, y, -dx[i], -dy[i], BLACK);

        // 1. 长连禁手 (超过5个) [cite: 1]
        // 这里必须明确：只有大于5才是长连，等于5是刚才的 CheckWin
        if (count > 5) return true;

        // 2. 统计三和四
        // 如果这行已经成5了，前面 CheckWin 会优先拦截返回 true，
        // 所以能走到这里说明 count != 5 (要么 <5，要么 >5已返回)
        if (count < 5) {
            int type = GetLineType(board, x, y, dx[i], dy[i]);
            if (type == 3) threeCount++;
            if (type == 4) fourCount++;
        }
    }

    // 三三禁手 [cite: 1]
    if (threeCount >= 2) return true;

    // 四四禁手 [cite: 1]
    if (fourCount >= 2) return true;

    return false;
}

// 智能分析：判断当前方向形成什么棋型 (3=活三, 4=四, 0=其他)
int CReferee::GetLineType(const CBoard& board, int x, int y, int dx, int dy) {
    int len = 1;
    int color = BLACK;

    // 正向延伸
    int i = 1;
    while (board.IsValid(x + dx*i, y + dy*i) && board.GetPiece(x + dx*i, y + dy*i) == color) {
        len++; i++;
    }
    Point p1 = { x + dx*i, y + dy*i };

    // 反向延伸
    int j = 1;
    while (board.IsValid(x - dx*j, y - dy*j) && board.GetPiece(x - dx*j, y - dy*j) == color) {
        len++; j++;
    }
    Point p2 = { x - dx*j, y - dy*j };

    // 检查两头空位
    bool open1 = (board.IsValid(p1.iX, p1.iY) && board.GetPiece(p1.iX, p1.iY) == EMPTY);
    bool open2 = (board.IsValid(p2.iX, p2.iY) && board.GetPiece(p2.iX, p2.iY) == EMPTY);

    // 冲四或活四
    if (len == 4) {
        if (open1 || open2) return 4;
    }

    // 活三：必须两头空
    if (len == 3) {
        if (open1 && open2) return 3;
    }

    return 0;
}