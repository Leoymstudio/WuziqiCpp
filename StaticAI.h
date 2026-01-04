#ifndef _STATICAI_H_
#define _STATICAI_H_

#include "Player.h"
#include "Board.h"

// 这是一个基于固定规则的“老师傅”AI，不具备学习能力，只用来当陪练
class CStaticAI : public CPlayer {
public:
    CStaticAI(int color);
    virtual Point MakeMove(CBoard& board) override;

private:
    int EvaluatePoint(CBoard& board, int x, int y);
    int GetLineScore(CBoard& board, int x, int y, int dx, int dy, int color);
};

#endif