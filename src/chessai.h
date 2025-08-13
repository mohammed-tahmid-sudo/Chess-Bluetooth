#ifndef CHESSAI_H
#define CHESSAI_H

#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <random>

struct Move {
    int fromRow, fromCol;
    int toRow, toCol;
    int score;
    std::string capturedPiece;
    
    Move() : fromRow(-1), fromCol(-1), toRow(-1), toCol(-1), score(0), capturedPiece("") {}
    Move(int fr, int fc, int tr, int tc) : fromRow(fr), fromCol(fc), toRow(tr), toCol(tc), score(0), capturedPiece("") {}
};

class ChessAI {
public:
    enum Difficulty {
        EASY = 2,     // Depth 2
        MEDIUM = 3,   // Depth 3
        HARD = 4,     // Depth 4
        EXPERT = 5    // Depth 5
    };
    
    ChessAI(Difficulty difficulty = MEDIUM);
    
    // Main AI function to get the best move
    Move getBestMove(const std::vector<std::vector<std::string>>& board, char aiColor);
    
    // Set AI difficulty
    void setDifficulty(Difficulty difficulty);
    
    // Get current difficulty
    Difficulty getDifficulty() const { return currentDifficulty; }
    
private:
    Difficulty currentDifficulty;
    std::mt19937 randomGenerator;
    
    // Minimax algorithm with alpha-beta pruning
    int minimax(std::vector<std::vector<std::string>> board, int depth, int alpha, int beta, 
                bool maximizing, char aiColor, Move& bestMove);
    
    // Board evaluation function
    int evaluateBoard(const std::vector<std::vector<std::string>>& board, char aiColor);
    
    // Get all possible moves for a color
    std::vector<Move> getAllPossibleMoves(const std::vector<std::vector<std::string>>& board, char color);
    
    // Check if a move is valid (uses existing chess logic)
    bool isValidMove(const std::vector<std::vector<std::string>>& board, int fromRow, int fromCol, int toRow, int toCol);
    
    // Make a move on the board (temporary for AI calculations)
    void makeMove(std::vector<std::vector<std::string>>& board, const Move& move);
    
    // Undo a move on the board
    void undoMove(std::vector<std::vector<std::string>>& board, const Move& move);
    
    // Get piece value for evaluation
    int getPieceValue(const std::string& piece);
    
    // Get piece color
    char getPieceColor(const std::string& piece);
    
    // Position evaluation tables for better play
    int getPositionValue(const std::string& piece, int row, int col);
    
    // Check if position is under attack
    bool isUnderAttack(const std::vector<std::vector<std::string>>& board, int row, int col, char attackingColor);
    
    // Order moves for better alpha-beta pruning
    void orderMoves(std::vector<Move>& moves, const std::vector<std::vector<std::string>>& board);
};

// External chess logic functions (from existing code)
extern bool CheckIfValidMove(int CurrentPosX, int CurrentPosY, int ExpectedPosX, int ExpectedPosY,
                            std::vector<std::vector<std::string>> Board, char Race, char name);
extern char Race(std::string str);

#endif // CHESSAI_H
