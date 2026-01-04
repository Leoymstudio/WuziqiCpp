#include "Board.h"
#include "Console.h"
#include "Player.h"
#include "Referee.h"
#include "AIPlayer.h" // [新增]
#include "StaticAI.h" // [新增] 引入陪练
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>      // [新增] 计时用

using namespace std;
// 一个只会随机下棋的“傻子”陪练
class CRandomPlayer : public CPlayer {
public:
    CRandomPlayer(int color) : CPlayer(color) { srand((unsigned)time(NULL)); }

    virtual Point MakeMove(CBoard& board) override {
        // 简单粗暴：随机找个空位
        while (true) {
            int x = rand() % BOARD_SIZE;
            int y = rand() % BOARD_SIZE;
            if (board.IsEmpty(x, y)) return {x, y};
        }
    }
};
string PointToString(Point p) {
    string s = "";
    s += (char)('A' + p.iX);
    s += to_string(p.iY + 1);
    return s;
}

int main() {
    system("chcp 65001");
    srand((unsigned)time(NULL)); // 随机种子

    // --- 1. 游戏模式选择 ---
    cout << "========================================" << endl;
    cout << "        五子棋大战 (C++ Console)        " << endl;
    cout << "========================================" << endl;
    cout << " 1. 人人对战 (P v P)" << endl;
    cout << " 2. 人机对战 (你执黑 vs NN白)" << endl;
    cout << " 3. 人机对战 (NN黑 vs 你执白)" << endl;
    cout << " 4. 训练模式 (NN vs 贪心AI) [快速进化]" << endl;
    cout << "========================================" << endl;
    cout << " 请选择模式 (1-4): ";

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
    } else if (mode ==1) {
        pBlack = new CHumanPlayer(BLACK);
        pWhite = new CHumanPlayer(WHITE);
    }else if (mode == 4) {
        cout << "=== 训练模式启动 ===" << endl;

        CAIPlayer* pNN = new CAIPlayer(BLACK); // NN 执黑
        pNN->SetTrainingMode(true);

        // 阶段一：虐菜训练 (建立基础自信)
        cout << "\n[阶段一] 基础训练：对抗随机乱下棋的对手..." << endl;
        cout << "目标：让神经网络学会基本的连珠规则" << endl;

        CRandomPlayer* pRandom = new CRandomPlayer(WHITE);
        int phase1Rounds = 1; // 练2000局
        int nnWins = 0;

        for (int i = 1; i <= phase1Rounds; i++) {
            CBoard board;
            bool bBlackTurn = true;
            bool bGameEnd = false;
            int moves = 0;

            while (true) {
                CPlayer* curr = bBlackTurn ? (CPlayer*)pNN : (CPlayer*)pRandom;
                Point p = curr->MakeMove(board);

                int color = bBlackTurn ? BLACK : WHITE;
                board.PlacePiece(p.iX, p.iY, color);
                moves++;

                if (CReferee::CheckWin(board, p.iX, p.iY)) {
                    if (bBlackTurn) { pNN->Learn(true); nnWins++; } // NN赢
                    else { pNN->Learn(false); } // NN输
                    bGameEnd = true;
                } else if (bBlackTurn && CReferee::CheckForbidden(board, p.iX, p.iY)) {
                    pNN->Learn(false); // 禁手输
                    bGameEnd = true;
                } else if (moves >= BOARD_SIZE * BOARD_SIZE) {
                     pNN->Learn(false); // 平局算输
                     bGameEnd = true;
                }

                if (bGameEnd) break;
                bBlackTurn = !bBlackTurn;
            }

            if (i % 100 == 0) {
                printf("阶段一进度: %d/%d | NN胜率: %.1f%% \n", i, phase1Rounds, (float)nnWins/i*100.0f);
            }
        }
        delete pRandom; // 傻子陪练下班了

        // 阶段二：进阶训练 (对抗贪心算法)
        cout << "\n[阶段二] 进阶训练：对抗贪心算法(StaticAI)..." << endl;
        cout << "注意：刚开始胜率可能会暴跌，这是正常的，它在适应高强度对抗。" << endl;

        CStaticAI* pStatic = new CStaticAI(WHITE);
        int phase2Rounds = 3000; // 再练3000局
        nnWins = 0; // 重置统计

        for (int i = 1; i <= phase2Rounds; i++) {
            CBoard board;
            bool bBlackTurn = true;
            bool bGameEnd = false;
            int moves = 0;

            while (true) {
                CPlayer* curr = bBlackTurn ? (CPlayer*)pNN : (CPlayer*)pStatic;
                Point p = curr->MakeMove(board);

                int color = bBlackTurn ? BLACK : WHITE;
                board.PlacePiece(p.iX, p.iY, color);
                moves++;

                if (CReferee::CheckWin(board, p.iX, p.iY)) {
                    if (bBlackTurn) { pNN->Learn(true); nnWins++; }
                    else { pNN->Learn(false); }
                    bGameEnd = true;
                } else if (bBlackTurn && CReferee::CheckForbidden(board, p.iX, p.iY)) {
                    pNN->Learn(false);
                    bGameEnd = true;
                } else if (moves >= BOARD_SIZE * BOARD_SIZE) {
                     pNN->Learn(false);
                     bGameEnd = true;
                }

                if (bGameEnd) break;
                bBlackTurn = !bBlackTurn;
            }
            if (i % 1000 == 0) {
                pNN->SaveBrain(); // 定期存档，防止断电白练
                printf(">>> 存档成功 (第 %d 局) <<<\n", i);
            }
            if (i % 100 == 0) {
                 printf("阶段二进度: %d/%d | NN胜率: %.1f%% \n", i, phase2Rounds, (float)nnWins/i*100.0f);
            }
        }
        pNN->SaveBrain();
        cout << "训练全部结束！模型已保存。" << endl;
        delete pNN;
        delete pStatic;

        cout << "按任意键退出...";
        cin.ignore(); cin.get();
        return 0;
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

            // ============== 学习逻辑开始 ==============
            CAIPlayer* pAI = nullptr;
            bool bAiWon = false;

            // 这里的 mode 变量必须是在 main 开头定义的那个
            // 确保你没有把它定义在 if 块里导致这里访问不到
            if (mode == 2) {
                // 模式2：人(黑) vs AI(白)。如果是黑赢(bIsBlackTurn=true)，则AI输
                pAI = dynamic_cast<CAIPlayer*>(pWhite);
                bAiWon = !bIsBlackTurn;
            }
            else if (mode == 3) {
                // 模式3：AI(黑) vs 人(白)。如果是黑赢，则AI赢
                pAI = dynamic_cast<CAIPlayer*>(pBlack);
                bAiWon = bIsBlackTurn;
            }

            if (pAI != nullptr) {
                pAI->Learn(bAiWon); // 这句话会触发 SaveWeights
            }
            // ============== 学习逻辑结束 ==============

            break; // 退出游戏大循环
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