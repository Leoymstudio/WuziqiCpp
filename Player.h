#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Board.h"
#include "Global.h"

class CPlayer {
protected:
    int m_iColor; // 玩家执棋颜色 (BLACK 或 WHITE)

public:
    CPlayer(int color);
    virtual ~CPlayer() {}

    // 纯虚函数：返回落子的坐标
    virtual Point MakeMove(CBoard& board) = 0;
};

// 人类玩家
class CHumanPlayer : public CPlayer {
public:
    CHumanPlayer(int color);
    virtual Point MakeMove(CBoard& board) override;
};

#endif