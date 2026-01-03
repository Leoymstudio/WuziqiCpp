#include "AIPlayer.h"
#include "Referee.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <fstream> // 文件流

using namespace std;

CAIPlayer::CAIPlayer(int color) : CPlayer(color) {
    srand((unsigned)time(NULL));
    m_strWeightFile = "ai_brain.txt"; // 大脑记忆文件
    LoadWeights(); // 出生时先读取记忆
}

// [新增] 读取记忆
void CAIPlayer::LoadWeights() {
    ifstream file(m_strWeightFile);
    if (file.is_open()) {
        file >> m_stWeights.iWin5 >> m_stWeights.iLive4 >> m_stWeights.iDash4
             >> m_stWeights.iLive3 >> m_stWeights.iLive2
             >> m_stWeights.fAttackFactor >> m_stWeights.fDefenseFactor;
        file.close();
    } else {
        // 如果没有记忆，使用默认值 (什么都不做)
    }
}

// [新增] 保存记忆
void CAIPlayer::SaveWeights() {
    ofstream file(m_strWeightFile);
    if (file.is_open()) {
        file << m_stWeights.iWin5 << " " << m_stWeights.iLive4 << " "
             << m_stWeights.iDash4 << " " << m_stWeights.iLive3 << " "
             << m_stWeights.iLive2 << " "
             << m_stWeights.fAttackFactor << " " << m_stWeights.fDefenseFactor;
        file.close();
    }
}

// [新增] 核心进化逻辑
void CAIPlayer::Learn(bool bAiWon) {
    if (bAiWon) {
        // 如果赢了，稍微增强一点自信（进攻欲望）
        // 或者是保持现状，认为当前策略是好的
        // 这里我们设计：赢了就微调，让它更激进一点点
        m_stWeights.fAttackFactor += 0.05f;
        if(m_stWeights.fAttackFactor > 2.0f) m_stWeights.fAttackFactor = 2.0f; // 封顶

        cout << "\n [AI复盘] 哈哈！我赢了！我觉得我的进攻策略很棒！(进攻欲望↑)" << endl;
    } else {
        // 如果输了，反思：是不是我太浪了？还是防守不够？
        // 策略：增加防守权重，降低进攻权重
        m_stWeights.fDefenseFactor += 0.1f;
        m_stWeights.fAttackFactor -= 0.05f;

        if (m_stWeights.fDefenseFactor > 2.5f) m_stWeights.fDefenseFactor = 2.5f;
        if (m_stWeights.fAttackFactor < 0.5f) m_stWeights.fAttackFactor = 0.5f;

        cout << "\n [AI复盘] 哎呀输了... 我下次会更注意防守的。 (防守意识↑)" << endl;
    }
    // 这一步很关键：把学到的新参数存进硬盘
    SaveWeights();
}

Point CAIPlayer::MakeMove(CBoard& board) {
    cout << endl << ">> 电脑正在思考 (攻:" << m_stWeights.fAttackFactor
         << " 防:" << m_stWeights.fDefenseFactor << ")..." << endl;

    clock_t start = clock();
    while (clock() - start < 500);

    int maxScore = -99999999;
    vector<Point> bestPoints;

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (!board.IsEmpty(x, y)) continue;

            if (m_iColor == BLACK) {
                board.PlacePiece(x, y, BLACK);
                if (CReferee::CheckForbidden(board, x, y)) {
                    board.UndoPiece(x, y);
                    continue;
                }
                board.UndoPiece(x, y);
            }

            int score = EvaluatePoint(board, x, y);
            if (score > maxScore) {
                maxScore = score;
                bestPoints.clear();
                bestPoints.push_back({x, y});
            } else if (score == maxScore) {
                bestPoints.push_back({x, y});
            }
        }
    }

    if (bestPoints.empty()) return {7, 7};
    int index = rand() % bestPoints.size();
    return bestPoints[index];
}

int CAIPlayer::EvaluatePoint(CBoard& board, int x, int y) {
    int totalScore = 0;
    int myColor = m_iColor;
    int enemyColor = (m_iColor == BLACK) ? WHITE : BLACK;

    // 使用读取到的权重参数进行计算
    int myScore = 0;
    myScore += GetLineScore(board, x, y, 1, 0, myColor);
    myScore += GetLineScore(board, x, y, 0, 1, myColor);
    myScore += GetLineScore(board, x, y, 1, 1, myColor);
    myScore += GetLineScore(board, x, y, 1, -1, myColor);

    // 乘上性格系数
    totalScore += (int)(myScore * m_stWeights.fAttackFactor);

    int enemyScore = 0;
    enemyScore += GetLineScore(board, x, y, 1, 0, enemyColor);
    enemyScore += GetLineScore(board, x, y, 0, 1, enemyColor);
    enemyScore += GetLineScore(board, x, y, 1, 1, enemyColor);
    enemyScore += GetLineScore(board, x, y, 1, -1, enemyColor);

    // 乘上性格系数
    totalScore += (int)(enemyScore * m_stWeights.fDefenseFactor);

    if (x >= 5 && x <= 9 && y >= 5 && y <= 9) totalScore += 10;

    return totalScore;
}

int CAIPlayer::GetLineScore(CBoard& board, int x, int y, int dx, int dy, int color) {
    int count = 1;
    int emptyEnds = 0;

    // ... (中间的扫描代码保持不变) ...
    // 这里为了节省篇幅省略，请使用上一版相同的扫描逻辑
    // 仅修改下面的 return 值，使用 m_stWeights 中的变量

    // 正向扫描...
    int i = 1;
    while (board.IsValid(x + dx*i, y + dy*i) && board.GetPiece(x + dx*i, y + dy*i) == color) { count++; i++; }
    if (board.IsValid(x + dx*i, y + dy*i) && board.GetPiece(x + dx*i, y + dy*i) == EMPTY) emptyEnds++;

    // 反向扫描...
    int j = 1;
    while (board.IsValid(x - dx*j, y - dy*j) && board.GetPiece(x - dx*j, y - dy*j) == color) { count++; j++; }
    if (board.IsValid(x - dx*j, y - dy*j) && board.GetPiece(x - dx*j, y - dy*j) == EMPTY) emptyEnds++;
    // ...

    // 使用变量代替硬编码
    if (count >= 5) return m_stWeights.iWin5;
    if (count == 4) {
        if (emptyEnds == 2) return m_stWeights.iLive4;
        if (emptyEnds == 1) return m_stWeights.iDash4;
    }
    if (count == 3) {
        if (emptyEnds == 2) return m_stWeights.iLive3;
        if (emptyEnds == 1) return m_stWeights.iLive2; // 眠三价值较低，近似活二
    }
    if (count == 2) {
        if (emptyEnds == 2) return m_stWeights.iLive2;
    }

    return count;
}