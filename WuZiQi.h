/**
 * @file WuZiQi.h
 * @brief Gomoku (Five-in-a-Row) CLI Game Header
 * @details Complete header file with OOP design and Hungarian notation
 * 
 * Classes: CPiece, CBoard, CReferee, CClock, CPlayer, CHumanPlayer, CAIPlayer, CGame
 */

#ifndef WUZIQI_H
#define WUZIQI_H

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <future>
#include <thread>
#include <random>
#include <algorithm>
#include <memory>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <utility>
#include <map>

// ============================================================================
// Constants
// ============================================================================
const int BOARD_SIZE = 15;
const int TIME_LIMIT_SECONDS = 15;
const int MAX_WARNINGS = 3;
const int MCTS_ITERATIONS = 1000;
const double UCB_CONSTANT = 1.414; // sqrt(2)

// ============================================================================
// Enumerations
// ============================================================================
enum class EPieceColor {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

enum class EGameResult {
    ONGOING,
    BLACK_WIN,
    WHITE_WIN,
    DRAW
};

enum class EGameMode {
    PVP,
    PVE
};

enum class ERPSChoice {
    ROCK = 0,
    PAPER = 1,
    SCISSORS = 2
};

// ============================================================================
// CPiece Class - Represents a stone on the board
// ============================================================================
class CPiece {
public:
    CPiece();
    CPiece(EPieceColor eColor, int iRow, int iCol);
    
    EPieceColor GetColor() const;
    int GetRow() const;
    int GetCol() const;
    
    void SetColor(EPieceColor eColor);
    void SetPosition(int iRow, int iCol);
    
private:
    EPieceColor m_eColor;
    int m_iRow;
    int m_iCol;
};

// ============================================================================
// CBoard Class - Manages the 15x15 game board
// ============================================================================
class CBoard {
public:
    CBoard();
    
    void Reset();
    bool SetPiece(int iRow, int iCol, EPieceColor eColor);
    EPieceColor GetPiece(int iRow, int iCol) const;
    bool IsValidPosition(int iRow, int iCol) const;
    bool IsEmpty(int iRow, int iCol) const;
    void Draw() const;
    
    void SetLastMove(int iRow, int iCol);
    std::pair<int, int> GetLastMove() const;
    bool HasLastMove() const;
    
    int GetEmptyCount() const;
    std::vector<std::pair<int, int>> GetEmptyPositions() const;
    
private:
    EPieceColor m_arrGrid[BOARD_SIZE][BOARD_SIZE];
    std::pair<int, int> m_pairLastMove;
    bool m_bHasLastMove;
    
    std::string GetIntersectionChar(int iRow, int iCol) const;
    std::string GetPieceChar(EPieceColor eColor, bool bIsLastMove) const;
};

// ============================================================================
// CReferee Class - Handles game rules and win/forbidden move detection
// ============================================================================
class CReferee {
public:
    CReferee();
    
    /**
     * @brief Check if a move is valid (within bounds and empty)
     */
    bool IsValidMove(const CBoard& board, int iRow, int iCol) const;
    
    /**
     * @brief Check if Black's move is forbidden
     * 
     * FORBIDDEN MOVE ALGORITHM (Black Only):
     * 1. If move creates exactly 5-in-a-row: NOT FORBIDDEN (5 beats all)
     * 2. Check Overline: >5 consecutive stones → FORBIDDEN
     * 3. Check Double-Four: ≥2 fours formed by this move → FORBIDDEN
     * 4. Check Double-Three: ≥2 open threes formed by this move → FORBIDDEN
     * 
     * @param board Current game board
     * @param iRow Row position
     * @param iCol Column position
     * @return true if the move is forbidden for Black
     */
    bool IsForbiddenMove(const CBoard& board, int iRow, int iCol) const;
    
    /**
     * @brief Check if placing a piece creates a win
     */
    bool CheckWin(const CBoard& board, int iRow, int iCol, EPieceColor eColor) const;
    
    /**
     * @brief Get the current game result
     */
    EGameResult GetGameResult(const CBoard& board) const;
    
private:
    // Direction vectors: horizontal, vertical, diagonal-down, diagonal-up
    static const int m_arrDirX[4];
    static const int m_arrDirY[4];
    
    /**
     * @brief Count consecutive stones in one direction
     */
    int CountConsecutive(const CBoard& board, int iRow, int iCol, 
                         int iDirX, int iDirY, EPieceColor eColor) const;
    
    /**
     * @brief Count stones in a line (both directions)
     */
    int CountLine(const CBoard& board, int iRow, int iCol, 
                  int iDirIdx, EPieceColor eColor) const;
    
    /**
     * @brief Check for overline (>5 in a row)
     */
    bool IsOverline(const CBoard& board, int iRow, int iCol) const;
    
    /**
     * @brief Count "fours" created by placing a stone (for double-four check)
     * A four is 4 consecutive stones that can become 5 with one more move
     */
    int CountFours(const CBoard& board, int iRow, int iCol) const;
    
    /**
     * @brief Count "open threes" created by placing a stone (for double-three check)
     * An open three is 3 consecutive stones with open ends that can become an open four
     */
    int CountOpenThrees(const CBoard& board, int iRow, int iCol) const;
    
    /**
     * @brief Check if a pattern forms a "four" in a specific direction
     */
    bool IsFourInDirection(const CBoard& board, int iRow, int iCol, int iDirIdx) const;
    
    /**
     * @brief Check if a pattern forms an "open three" in a specific direction
     */
    bool IsOpenThreeInDirection(const CBoard& board, int iRow, int iCol, int iDirIdx) const;
};

// ============================================================================
// CClock Class - Handles 15-second move timer
// ============================================================================
class CClock {
public:
    CClock();
    
    void Reset();
    void Start();
    void Stop();
    
    int GetElapsedSeconds() const;
    int GetRemainingSeconds() const;
    bool IsTimeUp() const;
    
    int GetWarnings() const;
    void AddWarning();
    bool IsMaxWarningsReached() const;
    
private:
    std::chrono::steady_clock::time_point m_tpStart;
    bool m_bIsRunning;
    int m_iWarnings;
};

// ============================================================================
// CPlayer Base Class
// ============================================================================
class CPlayer {
public:
    CPlayer(const std::string& strName, EPieceColor eColor);
    virtual ~CPlayer() = default;
    
    virtual std::pair<int, int> MakeMove(const CBoard& board, CClock& clock) = 0;
    
    std::string GetName() const;
    EPieceColor GetColor() const;
    
protected:
    std::string m_strName;
    EPieceColor m_eColor;
};

// ============================================================================
// CHumanPlayer Class - Human input handling
// ============================================================================
class CHumanPlayer : public CPlayer {
public:
    CHumanPlayer(const std::string& strName, EPieceColor eColor);
    
    std::pair<int, int> MakeMove(const CBoard& board, CClock& clock) override;
    
private:
    /**
     * @brief Get input with timeout using async
     * @return pair<bool, pair<int,int>> - success flag and coordinates
     */
    std::pair<bool, std::pair<int, int>> GetInputWithTimeout(int iTimeoutSeconds);
};

// ============================================================================
// MCTS Node for AI
// ============================================================================
struct SMCTSNode {
    int m_iRow;
    int m_iCol;
    int m_iVisits;
    double m_dWins;
    EPieceColor m_ePlayerColor;
    SMCTSNode* m_pParent;
    std::vector<std::unique_ptr<SMCTSNode>> m_vecChildren;
    std::vector<std::pair<int, int>> m_vecUntriedMoves;
    
    SMCTSNode(int iRow, int iCol, EPieceColor eColor, SMCTSNode* pParent = nullptr);
    
    SMCTSNode* SelectChild();
    SMCTSNode* AddChild(int iRow, int iCol, EPieceColor eColor);
    void Update(double dResult);
    double GetUCBValue() const;
};

// ============================================================================
// CAIPlayer Class - MCTS-based AI
// ============================================================================
class CAIPlayer : public CPlayer {
public:
    CAIPlayer(const std::string& strName, EPieceColor eColor);
    
    std::pair<int, int> MakeMove(const CBoard& board, CClock& clock) override;
    
private:
    CReferee m_referee;
    std::mt19937 m_rng;
    
    /**
     * @brief Run MCTS algorithm to find best move
     */
    std::pair<int, int> RunMCTS(const CBoard& board, int iIterations);
    
    /**
     * @brief Simulate a random game from current state
     */
    double Simulate(CBoard board, EPieceColor eCurrentPlayer);
    
    /**
     * @brief Get promising moves (prioritize moves near existing stones)
     */
    std::vector<std::pair<int, int>> GetPromisingMoves(const CBoard& board) const;
    
    /**
     * @brief Evaluate position heuristically
     */
    double EvaluatePosition(const CBoard& board, EPieceColor eColor) const;
};

// ============================================================================
// CGame Class - Main game controller
// ============================================================================
class CGame {
public:
    CGame();
    
    void Run();
    
private:
    std::unique_ptr<CBoard> m_pBoard;
    std::unique_ptr<CReferee> m_pReferee;
    std::unique_ptr<CPlayer> m_pPlayer1;
    std::unique_ptr<CPlayer> m_pPlayer2;
    CPlayer* m_pCurrentPlayer;
    CClock m_clock;
    EGameMode m_eGameMode;
    bool m_bGameOver;
    EGameResult m_eResult;
    
    void Initialize();
    void DisplayTitle();
    void SelectGameMode();
    void SetupPlayers();
    
    /**
     * @brief Rock-Paper-Scissors to determine who goes first
     * @return true if player 1 goes first, false if player 2
     */
    bool PlayRockPaperScissors();
    
    void GameLoop();
    void ProcessMove(int iRow, int iCol);
    void SwitchPlayer();
    void DisplayResult();
    void ClearScreen();
    
    ERPSChoice GetRPSChoice(const std::string& strPlayerName);
    std::string RPSChoiceToString(ERPSChoice eChoice);
    int CompareRPS(ERPSChoice eChoice1, ERPSChoice eChoice2);
};

#endif // WUZIQI_H
