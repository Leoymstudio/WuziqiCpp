#include "AIPlayer.h"
#include "Referee.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

CAIPlayer::CAIPlayer(int color) : CPlayer(color) {
    srand((unsigned)time(NULL)); // 初始化随机数种子
}

Point CAIPlayer::MakeMove(CBoard& board) {
    cout << endl << ">> 电脑正在思考..." << endl;
    
    // 简单的延时，假装在思考 (提升体验)
    // 实际上这个算法计算很快
    clock_t start = clock();
    while (clock() - start < 800); // 停顿0.8秒

    int maxScore = -99999999;
    vector<Point> bestPoints;

    // 遍历棋盘所有点
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (!board.IsEmpty(x, y)) continue;

            // 1. 如果我是黑棋，先检查是不是禁手
            // 如果是禁手，绝对不能走，分数为负无穷
            if (m_iColor == BLACK) {
                board.PlacePiece(x, y, BLACK); // 假装下子
                if (CReferee::CheckForbidden(board, x, y)) {
                    board.UndoPiece(x, y); // 撤销
                    continue; // 跳过这个点
                }
                board.UndoPiece(x, y); // 记得撤销
            }

            // 2. 评估该点分数
            int score = EvaluatePoint(board, x, y);

            // 3. 收集最高分
            if (score > maxScore) {
                maxScore = score;
                bestPoints.clear();
                bestPoints.push_back({x, y});
            } else if (score == maxScore) {
                bestPoints.push_back({x, y});
            }
        }
    }

    // 如果无处可下（极少见）
    if (bestPoints.empty()) return {7, 7};

    // 在最高分里随机选一个，防止走法太死板
    int index = rand() % bestPoints.size();
    return bestPoints[index];
}

// 评分函数：核心智能
int CAIPlayer::EvaluatePoint(CBoard& board, int x, int y) {
    int totalScore = 0;

    int myColor = m_iColor;
    int enemyColor = (m_iColor == BLACK) ? WHITE : BLACK;

    // 策略：进攻分 + 防守分
    // 通常防守比进攻重要，但如果能直接赢，进攻最重要
    
    // 1. 计算进攻分 (对自己有利)
    // 系数 1.0
    totalScore += GetLineScore(board, x, y, 1, 0, myColor); // 横
    totalScore += GetLineScore(board, x, y, 0, 1, myColor); // 竖
    totalScore += GetLineScore(board, x, y, 1, 1, myColor); // 斜
    totalScore += GetLineScore(board, x, y, 1, -1, myColor);// 反斜

    // 2. 计算防守分 (破坏敌人)
    // 系数 0.9 (或者更高，如果想让AI很龟缩)
    // 我们单独算，如果发现敌人有必杀棋，给极高分去堵
    int defenseScore = 0;
    defenseScore += GetLineScore(board, x, y, 1, 0, enemyColor);
    defenseScore += GetLineScore(board, x, y, 0, 1, enemyColor);
    defenseScore += GetLineScore(board, x, y, 1, 1, enemyColor);
    defenseScore += GetLineScore(board, x, y, 1, -1, enemyColor);

    // 如果防守分特别高（比如对面要赢了），我们优先堵
    // 这里的权重调节是 AI 强弱的关键
    if (defenseScore > 10000) totalScore += defenseScore + 1000; 
    else totalScore += defenseScore;

    // 中央位置稍微加点分 (开局抢占天元附近)
    if (x >= 5 && x <= 9 && y >= 5 && y <= 9) totalScore += 10;

    return totalScore;
}

// 计算某个方向的分数
int CAIPlayer::GetLineScore(CBoard& board, int x, int y, int dx, int dy, int color) {
    // 模拟连珠数
    int count = 1; // 假设自己下在这里
    int emptyEnds = 0;

    // 正向
    int i = 1;
    while (board.IsValid(x + dx*i, y + dy*i) && board.GetPiece(x + dx*i, y + dy*i) == color) {
        count++; i++;
    }
    if (board.IsValid(x + dx*i, y + dy*i) && board.GetPiece(x + dx*i, y + dy*i) == EMPTY) emptyEnds++;

    // 反向
    int j = 1;
    while (board.IsValid(x - dx*j, y - dy*j) && board.GetPiece(x - dx*j, y - dy*j) == color) {
        count++; j++;
    }
    if (board.IsValid(x - dx*j, y - dy*j) && board.GetPiece(x - dx*j, y - dy*j) == EMPTY) emptyEnds++;

    // --- 评分表 (Score Table) ---
    // 这里的数值可以随意调整，越大越优先
    
    if (count >= 5) return 100000; // 成5，绝杀/必救
    
    if (count == 4) {
        if (emptyEnds == 2) return 10000; // 活四 (011110) - 下一步必胜
        if (emptyEnds == 1) return 5000;  // 冲四 (011112) - 逼迫对方
    }
    
    if (count == 3) {
        if (emptyEnds == 2) return 1000;  // 活三 (01110) - 强力进攻
        if (emptyEnds == 1) return 100;   // 眠三
    }
    
    if (count == 2) {
        if (emptyEnds == 2) return 50;    // 活二
    }

    return count; // 只有1个子，分很低
}