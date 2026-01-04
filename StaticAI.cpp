#include "StaticAI.h"
#include "Referee.h"
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

CStaticAI::CStaticAI(int color) : CPlayer(color) {
    srand((unsigned)time(NULL));
}

Point CStaticAI::MakeMove(CBoard& board) {
    // 训练模式下不要打印 "电脑思考中..." 也不要 Sleep，追求极速
    
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

int CStaticAI::EvaluatePoint(CBoard& board, int x, int y) {
    int totalScore = 0;
    int myColor = m_iColor;
    int enemyColor = (m_iColor == BLACK) ? WHITE : BLACK;

    // 贪心算法的核心权重
    totalScore += GetLineScore(board, x, y, 1, 0, myColor);
    totalScore += GetLineScore(board, x, y, 0, 1, myColor);
    totalScore += GetLineScore(board, x, y, 1, 1, myColor);
    totalScore += GetLineScore(board, x, y, 1, -1, myColor);

    int defenseScore = 0;
    defenseScore += GetLineScore(board, x, y, 1, 0, enemyColor);
    defenseScore += GetLineScore(board, x, y, 0, 1, enemyColor);
    defenseScore += GetLineScore(board, x, y, 1, 1, enemyColor);
    defenseScore += GetLineScore(board, x, y, 1, -1, enemyColor);

    if (defenseScore > 10000) totalScore += defenseScore + 1000; 
    else totalScore += defenseScore;

    if (x >= 6 && x <= 8 && y >= 6 && y <= 8) totalScore += 10;

    return totalScore;
}

int CStaticAI::GetLineScore(CBoard& board, int x, int y, int dx, int dy, int color) {
    int count = 1; 
    int emptyEnds = 0;

    int i = 1;
    while (board.IsValid(x + dx*i, y + dy*i) && board.GetPiece(x + dx*i, y + dy*i) == color) { count++; i++; }
    if (board.IsValid(x + dx*i, y + dy*i) && board.GetPiece(x + dx*i, y + dy*i) == EMPTY) emptyEnds++;

    int j = 1;
    while (board.IsValid(x - dx*j, y - dy*j) && board.GetPiece(x - dx*j, y - dy*j) == color) { count++; j++; }
    if (board.IsValid(x - dx*j, y - dy*j) && board.GetPiece(x - dx*j, y - dy*j) == EMPTY) emptyEnds++;

    if (count >= 5) return 100000;
    if (count == 4) {
        if (emptyEnds == 2) return 10000;
        if (emptyEnds == 1) return 5000;
    }
    if (count == 3) {
        if (emptyEnds == 2) return 1000;
        if (emptyEnds == 1) return 100;
    }
    if (count == 2) {
        if (emptyEnds == 2) return 50;
    }
    return count;
}