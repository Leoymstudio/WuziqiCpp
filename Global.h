// Global.h
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

// 棋盘大小
const int BOARD_SIZE = 15;

// 棋子类型枚举
enum PieceType {
    EMPTY = 0,  // 空位
    BLACK = 1,  // 黑子
    WHITE = 2   // 白子
};

// 坐标结构体
struct Point {
    int iX;
    int iY;

    // 重载 == 操作符，方便比较
    bool operator==(const Point& other) const {
        return iX == other.iX && iY == other.iY;
    }
};

#endif