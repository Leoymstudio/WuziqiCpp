#ifndef _REFEREE_H_
#define _REFEREE_H_

#include "Board.h"
#include "Global.h"

class CReferee {
public:
    // 检查是否胜利 (五连)
    static bool CheckWin(const CBoard& board, int x, int y);

    // 检查是否触发黑棋禁手 (返回 true 表示犯规)
    static bool CheckForbidden(const CBoard& board, int x, int y);

private:
    // 辅助函数：计算某个方向的连子数
    static int CountConsecutive(const CBoard& board, int x, int y, int dx, int dy, int color);
    
    // 辅助函数：判断某个方向构成的棋型 (3=活三, 4=四, 0=其他)
    static int GetLineType(const CBoard& board, int x, int y, int dx, int dy);
};

#endif