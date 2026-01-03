#ifndef _AIPLAYER_H_
#define _AIPLAYER_H_

#include "Player.h"
#include "Board.h"

class CAIPlayer : public CPlayer {
public:
    CAIPlayer(int color);
    
    // 覆写 MakeMove，AI 不需要键盘输入，而是自己计算
    virtual Point MakeMove(CBoard& board) override;

private:
    // 核心算法：评估某个点的价值
    int EvaluatePoint(CBoard& board, int x, int y);
    
    // 辅助：计算某个方向的连子价值
    int GetLineScore(CBoard& board, int x, int y, int dx, int dy, int targetColor);
};

#endif