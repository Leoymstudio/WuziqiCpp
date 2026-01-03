#include "Board.h"
#include "Console.h"
#include "Player.h"
#include "Referee.h"
#include "AIPlayer.h" // [新增]
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>      // [新增] 计时用

using namespace std;

string PointToString(Point p) {
    string s = "";
    s += (char)('A' + p.iX);
    s += to_string(p.iY + 1);
    return s;
}

int main() {
    system("chcp 65001");

    // --- 1. 游戏模式选择 ---
    cout << "========================================" << endl;
    cout << "        五子棋大战 (C++ Console)        " << endl;
    cout << "========================================" << endl;
    cout << " 1. 人人对战 (P v P)" << endl;
    cout << " 2. 人机对战 (P v E) - 你执黑" << endl;
    cout << " 3. 人机对战 (P v E) - 你执白" << endl;
    cout << "========================================" << endl;
    cout << " 请选择模式 (1-3): ";

    int mode;
    cin >> mode;

    CBoard board;
    CPlayer* pBlack = nullptr;
    CPlayer* pWhite = nullptr;

    if (mode == 2) {
        pBlack = new CHumanPlayer(BLACK);
        pWhite = new CAIPlayer(WHITE);
    } else if (mode == 3) {
        pBlack = new CAIPlayer(BLACK);
        pWhite = new CHumanPlayer(WHITE);
    } else {
        pBlack = new CHumanPlayer(BLACK);
        pWhite = new CHumanPlayer(WHITE);
    }

    bool bIsBlackTurn = true;
    int round = 1;
    vector<string> history;

    // 计时相关：超时警告次数
    int blackWarnings = 0;
    int whiteWarnings = 0;

    // --- 2. 游戏主循环 ---
    while (true) {
        cout << "\n========================================\n";
        cout << " 第 " << round << " 手  |  当前执子: "
             << (bIsBlackTurn ? "黑 (Black)" : "白 (White)") << endl;
        cout << "========================================\n";

        board.Draw();

        if (!history.empty()) {
            cout << "\n[历史]: " << history.back() << endl;
        }

        CPlayer* curr = bIsBlackTurn ? pBlack : pWhite;
        Point p;
        bool bWin = false;

        // --- 计时开始 ---
        clock_t startTime = clock();

        while(true) {
            p = curr->MakeMove(board);

            // --- 计时结束与判断 ---
            // 只有人类玩家需要判断超时，AI一般瞬间完成
            // 为了简化代码，全部判断
            double elapsed = (double)(clock() - startTime) / CLOCKS_PER_SEC;

            // 规则：每手不超过15秒
            if (elapsed > 15.0) {
                int& warnings = bIsBlackTurn ? blackWarnings : whiteWarnings;
                warnings++;
                cout << "\n [超时!] 本步耗时 " << elapsed << " 秒 (限时15秒)。" << endl;
                cout << " [警告] 当前累计警告次数: " << warnings << "/3" << endl;

                if (warnings >= 3) {
                    cout << "\n########################################\n";
                    cout << " 比赛结束！ " << (bIsBlackTurn ? "白方" : "黑方") << " 获胜！";
                    cout << "\n 原因：对手超时违例满 3 次。";
                    cout << "\n########################################\n";
                    goto GAME_END; // 直接结束
                }
            }

            int color = bIsBlackTurn ? BLACK : WHITE;
            board.PlacePiece(p.iX, p.iY, color);

            // A. 检查胜利
            if (CReferee::CheckWin(board, p.iX, p.iY)) {
                bWin = true;
                break;
            }

            // B. 检查禁手
            if (bIsBlackTurn) {
                if (CReferee::CheckForbidden(board, p.iX, p.iY)) {
                    cout << "\n [警告] 禁手点 (长连/三三/四四)！请重下。" << endl;
                    board.UndoPiece(p.iX, p.iY);

                    // 注意：重下不重置计时器，依然算思考时间
                    continue;
                }
            }
            break;
        }

        if (bWin) {
            board.Draw();
            cout << "\n########################################\n";
            cout << " 比赛结束！ " << (bIsBlackTurn ? "黑方" : "白方") << " 获胜！";
            if (bIsBlackTurn) cout << "(五连)";
            else cout << "(五连或长连)";
            cout << "\n########################################\n";
            // ... (在打印完获胜信息后) ...

            cout << "\n########################################\n";

            // --- 触发 AI 学习 ---
            // 我们需要知道谁是AI。
            // mode 2: 人(黑) vs AI(白)
            // mode 3: AI(黑) vs 人(白)

            CAIPlayer* pAI = nullptr;
            bool bAiWon = false;

            if (mode == 2) {
                // AI 是白棋
                pAI = (CAIPlayer*)pWhite; // 强转
                bAiWon = !bIsBlackTurn;   // 如果最后一步是黑下的(导致赢)，那AI没赢。等等，这里逻辑要理清

                // 胜负逻辑：
                // 如果 bIsBlackTurn 为 true (当前是黑回合)，说明上一手是白下的，且白赢了。
                // 这里的 bIsBlackTurn 已经被 loop 末尾取反过了吗？
                // 我们的代码在 break 前还没有取反。
                // 所以：如果 bIsBlackTurn == true，说明黑棋赢了。

                bAiWon = !bIsBlackTurn; // 黑赢(true) -> AI白输(false)
            }
            else if (mode == 3) {
                // AI 是黑棋
                pAI = (CAIPlayer*)pBlack;
                bAiWon = bIsBlackTurn;
            }

            if (pAI != nullptr) {
                pAI->Learn(bAiWon);
            }
            break;
        }

        string moveStr = (bIsBlackTurn ? "黑: " : "白: ") + PointToString(p);
        history.push_back(moveStr);

        bIsBlackTurn = !bIsBlackTurn;
        round++;
    }



GAME_END:
    // 清理内存
    delete pBlack;
    delete pWhite;

    cout << "按任意键退出...";
    cin.ignore(); cin.get();
    return 0;
}