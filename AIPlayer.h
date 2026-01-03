#ifndef _AIPLAYER_H_
#define _AIPLAYER_H_

#include "Player.h"
#include "Board.h"
#include <string>

// AI的“大脑参数”
struct AIWeights {
    // 基础分值
    int iWin5 = 100000;    // 连5
    int iLive4 = 10000;    // 活4
    int iDash4 = 5000;     // 冲4
    int iLive3 = 1000;     // 活3
    int iLive2 = 100;      // 活2

    // 性格参数
    float fAttackFactor = 1.0f;  // 进攻系数 (越高越爱进攻)
    float fDefenseFactor = 1.0f; // 防守系数 (越高越怕死)
};

class CAIPlayer : public CPlayer {
public:
    CAIPlayer(int color);

    virtual Point MakeMove(CBoard& board) override;

    // [新增] 学习功能：根据胜负调整参数
    void Learn(bool bAiWon);

private:
    AIWeights m_stWeights; // 当前的权重
    std::string m_strWeightFile; // 记忆文件路径

    int EvaluatePoint(CBoard& board, int x, int y);
    int GetLineScore(CBoard& board, int x, int y, int dx, int dy, int color);

    // 文件操作
    void LoadWeights();
    void SaveWeights();
};

#endif