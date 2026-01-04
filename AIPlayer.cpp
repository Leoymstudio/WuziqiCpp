#include "AIPlayer.h"
#include "Referee.h"
#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;

// 神经网络结构：8个输入 -> 12个隐藏神经元 -> 1个输出
CAIPlayer::CAIPlayer(int color) : CPlayer(color), m_bTrainingMode(false) {
    srand((unsigned)time(NULL));
    // 8个特征: [我方活4, 冲4, 活3, 活2, 敌方活4, 冲4, 活3, 活2]
    m_pBrain = new NeuralNet(8, 12, 1);
    cout<<"Loading AI Player"<<endl;
    m_pBrain->Load("nn_weights.txt");
}

CAIPlayer::~CAIPlayer() {
    delete m_pBrain;
}
// [新增] 紧急情况检查器
Point CAIPlayer::CheckEmergency(CBoard& board) {
    int myColor = m_iColor;
    int enemyColor = (m_iColor == BLACK) ? WHITE : BLACK;

    // 临时变量，用于记录防守点
    Point defensePoint = {-1, -1};
    // 防守优先级：堵冲四 > 堵活三。但如果我们能直接赢，那就忽略防守。

    // 遍历所有空位
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (!board.IsEmpty(x, y)) continue;

            // 1. 检查：我这一步能不能直接赢？ (连五)
            board.PlacePiece(x, y, myColor);
            bool win = CReferee::CheckWin(board, x, y);
            board.UndoPiece(x, y);
            if (win) return {x, y}; // 绝杀！直接下！

            // 2. 检查：敌人下这一步会不会赢？ (敌方连五)
            // 如果敌人能连五，我必须堵！
            board.PlacePiece(x, y, enemyColor);
            bool enemyWin = CReferee::CheckWin(board, x, y);
            board.UndoPiece(x, y);
            if (enemyWin) {
                defensePoint = {x, y}; // 记下来，必须堵（除非我能赢，上面已处理）
                return defensePoint;   // 发现对面要赢，最高优先级防守
            }

            // 3. 检查：敌人是否有“活四”或者“冲四” (下一步必赢)
            // 我们利用之前的 GetPatternType 逻辑
            int dx[] = { 1, 0, 1, 1 };
            int dy[] = { 0, 1, 1, -1 };

            // 检查敌方威胁
            board.PlacePiece(x, y, enemyColor); // 假设敌人下这
            for(int i=0; i<4; i++) {
                int type = GetPatternType(board, x, y, dx[i], dy[i], enemyColor);
                if (type == 2 || type == 3) {
                    // 敌人形成了活四(2)或冲四(3)，这步棋是致命威胁点
                    defensePoint = {x, y};
                }
            }
            board.UndoPiece(x, y);
        }
    }

    return defensePoint; // 如果找到了必须堵的点，返回；否则返回 {-1, -1}
}
Point CAIPlayer::MakeMove(CBoard& board) {
    if (!m_bTrainingMode) {
        cout << endl << ">> 神经网络正在计算..." << endl;
        clock_t start = clock();
        while (clock() - start < 500);
    }
    // --- 步骤 0: 紧急检查 (Logic Filter) ---
    // 在动用神经网络之前，先看看有没有能够直接赢，或者必须堵的棋
    Point emergency = CheckEmergency(board);
    if (emergency.iX != -1) {
        if (!m_bTrainingMode) cout << " [AI直觉] 发现关键点 (绝杀或必救)！" << endl;

        // 即使是强制落子，我们也把它记入记忆，让神经网络学习这一步是“好棋”
        // 构造这一步的特征
        vector<double> f = GetFeatures(board, emergency.iX, emergency.iY);
        m_matchMemory.push_back({f});

        return emergency;
    }

    double maxScore = -9999.0;
    vector<Point> bestPoints;

    // 保存这一步的最佳特征，稍后存入记忆
    vector<double> bestFeatures;

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

            // 1. 获取特征
            vector<double> features = GetFeatures(board, x, y);
            // [调试用] 看看眼睛是不是瞎的
            // if (x == 0 && y == 0) {
            //     cout << "\n[调试] 坐标(0,0)的特征 check: " << endl;
            //     cout << "   我方活4: " << features[0] << endl;
            //     cout << "   我方活3: " << features[2] << endl;
            //     cout << "   敌方活4: " << features[4] << endl;
            //     // 强制刷新缓冲区，确保打印出来
            //     cout << flush;
            // }

            // 2. 神经网络预测胜率 (0.0 ~ 1.0)
            double score = m_pBrain->Forward(features);

            // 额外加分：优先抢占天元附近 (辅助引导)
            if (x >= 6 && x <= 8 && y >= 6 && y <= 8) score += 0.05;

            if (score > maxScore) {
                maxScore = score;
                bestPoints.clear();
                bestPoints.push_back({x, y});
                bestFeatures = features;
            } else if (abs(score - maxScore) < 0.0001) {
                bestPoints.push_back({x, y});
            }
        }
    }

    Point bestMove = {7, 7};
    if (!bestPoints.empty()) {
        int index = rand() % bestPoints.size();
        bestMove = bestPoints[index];
    }

    // 3. 将这一步的特征记入“海马体”，比赛结束后复盘用
    if (!bestFeatures.empty()) {
        m_matchMemory.push_back({bestFeatures});
    }

    return bestMove;
}

// 提取特征向量 (这是 AI 的眼睛)
vector<double> CAIPlayer::GetFeatures(CBoard& board, int x, int y) {
    // 特征向量初始化为 0
    // [0-3]: 我方(活4, 冲4, 活3, 活2)
    // [4-7]: 敌方(活4, 冲4, 活3, 活2)
    vector<double> features(8, 0.0);

    int myColor = m_iColor;
    int enemyColor = (m_iColor == BLACK) ? WHITE : BLACK;

    int dx[] = { 1, 0, 1, 1 };
    int dy[] = { 0, 1, 1, -1 };

    // --- A. 评估我方：如果我下在这里，能形成什么？ ---
    board.PlacePiece(x, y, myColor); // 模拟落子

    for(int i=0; i<4; i++) {
        int type = GetPatternType(board, x, y, dx[i], dy[i], myColor);

        // 连五归为活四一类 (必胜)
        if (type == 1) features[0] += 1.0;      // 连五 -> 算作超级活四
        else if (type == 2) features[0] += 1.0; // 活四
        else if (type == 3) features[1] += 1.0; // 冲四
        else if (type == 4) features[2] += 1.0; // 活三
        else if (type == 5) features[3] += 1.0; // 活二
    }
    board.UndoPiece(x, y); // 撤销

    // --- B. 评估敌方：如果敌人下在这里，能形成什么？(即我的防守价值) ---
    board.PlacePiece(x, y, enemyColor); // 模拟敌人落子

    for(int i=0; i<4; i++) {
        int type = GetPatternType(board, x, y, dx[i], dy[i], enemyColor);

        if (type == 1) features[4] += 1.0;      // 敌连五
        else if (type == 2) features[4] += 1.0; // 敌活四
        else if (type == 3) features[5] += 1.0; // 敌冲四
        else if (type == 4) features[6] += 1.0; // 敌活三
        else if (type == 5) features[7] += 1.0; // 敌活二
    }
    board.UndoPiece(x, y); // 撤销

    // 归一化处理 (可选，但神经网络通常喜欢 0.0~1.0 的输入)
    // 这里我们可以不做严格归一化，因为棋盘上同时出现两个活四的情况很少，值不会很大
    return features;
}

// 2. 辅助函数：判断棋型
// 返回值：0=无, 1=连五, 2=活四, 3=冲四, 4=活三, 5=活二
int CAIPlayer::GetPatternType(CBoard& board, int x, int y, int dx, int dy, int color) {
    int count = 1; // 包含当前点
    int emptyEnds = 0;

    // 正向扫描
    int i = 1;
    while (board.IsValid(x + dx*i, y + dy*i) && board.GetPiece(x + dx*i, y + dy*i) == color) {
        count++; i++;
    }
    // 检查正向尽头是否为空
    if (board.IsValid(x + dx*i, y + dy*i) && board.GetPiece(x + dx*i, y + dy*i) == EMPTY) {
        emptyEnds++;
    }

    // 反向扫描
    int j = 1;
    while (board.IsValid(x - dx*j, y - dy*j) && board.GetPiece(x - dx*j, y - dy*j) == color) {
        count++; j++;
    }
    // 检查反向尽头是否为空
    if (board.IsValid(x - dx*j, y - dy*j) && board.GetPiece(x - dx*j, y - dy*j) == EMPTY) {
        emptyEnds++;
    }

    // --- 类型判定逻辑 ---

    if (count >= 5) return 1; // 连五 (最高优先级)

    if (count == 4) {
        if (emptyEnds == 2) return 2; // 活四 (011110)
        if (emptyEnds == 1) return 3; // 冲四 (011112 或 211110)
    }

    if (count == 3) {
        if (emptyEnds == 2) return 4; // 活三 (01110)
        // 眠三 (冲三) 暂时不作为主要特征，或者你可以归类为弱威胁
        // if (emptyEnds == 1) return ...;
    }

    if (count == 2) {
        if (emptyEnds == 2) return 5; // 活二 (0110)
    }

    return 0; // 其他杂乱棋型
}
// [新增] 专门的保存函数
void CAIPlayer::SaveBrain() {
    if (m_pBrain) {
        m_pBrain->Save("nn_weights.txt");
    }
}
// 进化：赛后复盘
void CAIPlayer::Learn(bool bAiWon) {
    double target = bAiWon ? 1.0 : 0.0;

    // cout << "\n [AI复盘] 启动反向传播 (Backpropagation)..." << endl;
    // cout << " [AI复盘] 本局步数: " << m_matchMemory.size() << " 期望目标: " << target << endl;

    // 遍历本局下的每一步，告诉神经网络：
    // "如果在这种局面(Features)下，你走了这一步，最后导致了(Win/Loss)"
    for (const auto& move : m_matchMemory) {
        m_pBrain->Train(move.features, target);
    }

    // m_pBrain->Save("nn_weights.txt");
    m_matchMemory.clear(); // 清空记忆准备下一场
}