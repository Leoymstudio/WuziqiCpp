#ifndef _BOARD_H_
#define _BOARD_H_

#include "Global.h"

class CBoard {
private:
    int m_iGrid[BOARD_SIZE][BOARD_SIZE]; // 棋盘数据
    Point m_stLastMove;                  // 记录最后一步棋的位置

public:
    CBoard();
    
    // 重置棋盘
    void Reset();
    
    // 绘制整个棋盘
    void Draw();
    
    // 绘制棋盘上的某一个点 (局部刷新，避免闪烁)
    void DrawNode(int x, int y);

    // 下子操作
    void PlacePiece(int x, int y, int type);
    
    // 悔棋/回溯操作 (AI思考时会用到)
    void UndoPiece(int x, int y);
    
    // 获取某点的棋子状态
    int GetPiece(int x, int y) const;
    
    // 检查坐标是否越界
    bool IsValid(int x, int y) const;
    
    // 检查是否为空
    bool IsEmpty(int x, int y) const;
    
    // 获取最后一步的位置
    Point GetLastMove() const;
};

#endif