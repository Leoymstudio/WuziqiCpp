#ifndef _AIPLAYER_H_
#define _AIPLAYER_H_

#include "Player.h"
#include "Board.h"
#include "NeuralNet.h"
#include <vector>

class CAIPlayer : public CPlayer {
public:
    CAIPlayer(int color);
    ~CAIPlayer();
    void SaveBrain();
    virtual Point MakeMove(CBoard& board) override;
    void Learn(bool bAiWon);
    void SetTrainingMode(bool bTrain) { m_bTrainingMode = bTrain; }

private:
    NeuralNet* m_pBrain;
    bool m_bTrainingMode = false;
    struct MoveMemory {
        std::vector<double> features;
    };
    std::vector<MoveMemory> m_matchMemory;

    // 获取特征向量 (神经网络的输入)
    std::vector<double> GetFeatures(CBoard& board, int x, int y);

    // [重点新增] 判断某个方向的具体棋型类型
    // 返回值：0=无/杂乱, 1=连五, 2=活四, 3=冲四, 4=活三, 5=活二
    int GetPatternType(CBoard& board, int x, int y, int dx, int dy, int color);
    // 检查是否有必杀点 (自己赢，或者必须堵的对方赢)
    Point CheckEmergency(CBoard& board);
};

#endif