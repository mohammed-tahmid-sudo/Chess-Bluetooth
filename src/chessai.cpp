#include "chessai.h"
#include <cctype>
#include <climits>
#include <chrono>

ChessAI::ChessAI(Difficulty difficulty) : currentDifficulty(difficulty) {
    // Initialize random generator with current time
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    randomGenerator.seed(seed);
}

void ChessAI::setDifficulty(Difficulty difficulty) {
    currentDifficulty = difficulty;
}

Move ChessAI::getBestMove(const std::vector<std::vector<std::string>>& board, char aiColor) {
    Move bestMove;
    int bestScore = (aiColor == 'b') ? INT_MAX : INT_MIN;
    
    // Get all possible moves for AI
    std::vector<Move> possibleMoves = getAllPossibleMoves(board, aiColor);
    
    if (possibleMoves.empty()) {
        return bestMove; // No moves available
    }
    
    // Order moves for better pruning
    orderMoves(possibleMoves, board);
    
    int alpha = INT_MIN;
    int beta = INT_MAX;
    
    for (Move& move : possibleMoves) {
        std::vector<std::vector<std::string>> tempBoard = board;
        makeMove(tempBoard, move);
        
        int score = minimax(tempBoard, currentDifficulty - 1, alpha, beta, 
                           aiColor == 'w', aiColor, move);
        
        move.score = score;
        
        if (aiColor == 'b') { // AI is black (minimizing)
            if (score < bestScore) {
                bestScore = score;
                bestMove = move;
            }
            beta = std::min(beta, score);
        } else { // AI is white (maximizing)
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
            alpha = std::max(alpha, score);
        }
        
        if (beta <= alpha) break; // Alpha-beta pruning
    }
    
    // Add some randomness for same-score moves to make AI less predictable
    std::vector<Move> bestMoves;
    for (const Move& move : possibleMoves) {
        if (move.score == bestScore) {
            bestMoves.push_back(move);
        }
    }
    
    if (!bestMoves.empty()) {
        std::uniform_int_distribution<int> dist(0, bestMoves.size() - 1);
        bestMove = bestMoves[dist(randomGenerator)];
    }
    
    return bestMove;
}

int ChessAI::minimax(std::vector<std::vector<std::string>> board, int depth, int alpha, int beta, 
                     bool maximizing, char aiColor, Move& bestMove) {
    if (depth == 0) {
        return evaluateBoard(board, aiColor);
    }
    
    char currentColor = maximizing ? 'w' : 'b';
    std::vector<Move> moves = getAllPossibleMoves(board, currentColor);
    
    if (moves.empty()) {
        // No moves available - check for checkmate or stalemate
        return evaluateBoard(board, aiColor);
    }
    
    orderMoves(moves, board);
    
    if (maximizing) {
        int maxScore = INT_MIN;
        for (const Move& move : moves) {
            makeMove(board, move);
            Move dummy;
            int score = minimax(board, depth - 1, alpha, beta, false, aiColor, dummy);
            undoMove(board, move);
            
            maxScore = std::max(maxScore, score);
            alpha = std::max(alpha, score);
            
            if (beta <= alpha) break; // Alpha-beta pruning
        }
        return maxScore;
    } else {
        int minScore = INT_MAX;
        for (const Move& move : moves) {
            makeMove(board, move);
            Move dummy;
            int score = minimax(board, depth - 1, alpha, beta, true, aiColor, dummy);
            undoMove(board, move);
            
            minScore = std::min(minScore, score);
            beta = std::min(beta, score);
            
            if (beta <= alpha) break; // Alpha-beta pruning
        }
        return minScore;
    }
}

int ChessAI::evaluateBoard(const std::vector<std::vector<std::string>>& board, char aiColor) {
    int score = 0;
    
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const std::string& piece = board[row][col];
            if (!piece.empty()) {
                char pieceColor = getPieceColor(piece);
                int pieceValue = getPieceValue(piece);
                int positionValue = getPositionValue(piece, row, col);
                
                int totalValue = pieceValue + positionValue;
                
                if (pieceColor == aiColor) {
                    score += totalValue;
                } else {
                    score -= totalValue;
                }
            }
        }
    }
    
    return score;
}

std::vector<Move> ChessAI::getAllPossibleMoves(const std::vector<std::vector<std::string>>& board, char color) {
    std::vector<Move> moves;
    
    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            const std::string& piece = board[fromRow][fromCol];
            if (!piece.empty() && getPieceColor(piece) == color) {
                for (int toRow = 0; toRow < 8; ++toRow) {
                    for (int toCol = 0; toCol < 8; ++toCol) {
                        if (isValidMove(board, fromRow, fromCol, toRow, toCol)) {
                            Move move(fromRow, fromCol, toRow, toCol);
                            move.capturedPiece = board[toRow][toCol];
                            moves.push_back(move);
                        }
                    }
                }
            }
        }
    }
    
    return moves;
}

bool ChessAI::isValidMove(const std::vector<std::vector<std::string>>& board, int fromRow, int fromCol, int toRow, int toCol) {
    if (board[fromRow][fromCol].empty()) return false;
    
    std::string piece = board[fromRow][fromCol];
    char race = getPieceColor(piece);
    char name = std::tolower(piece[0]);
    
    return CheckIfValidMove(fromRow, fromCol, toRow, toCol, 
                           const_cast<std::vector<std::vector<std::string>>&>(board), race, name);
}

void ChessAI::makeMove(std::vector<std::vector<std::string>>& board, const Move& move) {
    board[move.toRow][move.toCol] = board[move.fromRow][move.fromCol];
    board[move.fromRow][move.fromCol] = "";
}

void ChessAI::undoMove(std::vector<std::vector<std::string>>& board, const Move& move) {
    board[move.fromRow][move.fromCol] = board[move.toRow][move.toCol];
    board[move.toRow][move.toCol] = move.capturedPiece;
}

int ChessAI::getPieceValue(const std::string& piece) {
    if (piece.empty()) return 0;
    
    char p = std::tolower(piece[0]);
    switch (p) {
        case 'p': return 100;   // Pawn
        case 'n': return 320;   // Knight
        case 'b': return 330;   // Bishop
        case 'r': return 500;   // Rook
        case 'q': return 900;   // Queen
        case 'k': return 20000; // King
        default: return 0;
    }
}

char ChessAI::getPieceColor(const std::string& piece) {
    if (piece.empty()) return 'n';
    return std::isupper(piece[0]) ? 'w' : 'b';
}

int ChessAI::getPositionValue(const std::string& piece, int row, int col) {
    if (piece.empty()) return 0;
    
    char p = std::tolower(piece[0]);
    
    // Position value tables (from white's perspective)
    static const int pawnTable[8][8] = {
        {0,  0,  0,  0,  0,  0,  0,  0},
        {50, 50, 50, 50, 50, 50, 50, 50},
        {10, 10, 20, 30, 30, 20, 10, 10},
        {5,  5, 10, 25, 25, 10,  5,  5},
        {0,  0,  0, 20, 20,  0,  0,  0},
        {5, -5,-10,  0,  0,-10, -5,  5},
        {5, 10, 10,-20,-20, 10, 10,  5},
        {0,  0,  0,  0,  0,  0,  0,  0}
    };
    
    static const int knightTable[8][8] = {
        {-50,-40,-30,-30,-30,-30,-40,-50},
        {-40,-20,  0,  0,  0,  0,-20,-40},
        {-30,  0, 10, 15, 15, 10,  0,-30},
        {-30,  5, 15, 20, 20, 15,  5,-30},
        {-30,  0, 15, 20, 20, 15,  0,-30},
        {-30,  5, 10, 15, 15, 10,  5,-30},
        {-40,-20,  0,  5,  5,  0,-20,-40},
        {-50,-40,-30,-30,-30,-30,-40,-50}
    };
    
    static const int bishopTable[8][8] = {
        {-20,-10,-10,-10,-10,-10,-10,-20},
        {-10,  0,  0,  0,  0,  0,  0,-10},
        {-10,  0,  5, 10, 10,  5,  0,-10},
        {-10,  5,  5, 10, 10,  5,  5,-10},
        {-10,  0, 10, 10, 10, 10,  0,-10},
        {-10, 10, 10, 10, 10, 10, 10,-10},
        {-10,  5,  0,  0,  0,  0,  5,-10},
        {-20,-10,-10,-10,-10,-10,-10,-20}
    };
    
    static const int rookTable[8][8] = {
        { 0,  0,  0,  0,  0,  0,  0,  0},
        { 5, 10, 10, 10, 10, 10, 10,  5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        {-5,  0,  0,  0,  0,  0,  0, -5},
        { 0,  0,  0,  5,  5,  0,  0,  0}
    };
    
    static const int queenTable[8][8] = {
        {-20,-10,-10, -5, -5,-10,-10,-20},
        {-10,  0,  0,  0,  0,  0,  0,-10},
        {-10,  0,  5,  5,  5,  5,  0,-10},
        { -5,  0,  5,  5,  5,  5,  0, -5},
        {  0,  0,  5,  5,  5,  5,  0, -5},
        {-10,  5,  5,  5,  5,  5,  0,-10},
        {-10,  0,  5,  0,  0,  0,  0,-10},
        {-20,-10,-10, -5, -5,-10,-10,-20}
    };
    
    static const int kingTable[8][8] = {
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-30,-40,-40,-50,-50,-40,-40,-30},
        {-20,-30,-30,-40,-40,-30,-30,-20},
        {-10,-20,-20,-20,-20,-20,-20,-10},
        { 20, 20,  0,  0,  0,  0, 20, 20},
        { 20, 30, 10,  0,  0, 10, 30, 20}
    };
    
    int value = 0;
    bool isWhite = std::isupper(piece[0]);
    int actualRow = isWhite ? row : (7 - row);
    
    switch (p) {
        case 'p': value = pawnTable[actualRow][col]; break;
        case 'n': value = knightTable[actualRow][col]; break;
        case 'b': value = bishopTable[actualRow][col]; break;
        case 'r': value = rookTable[actualRow][col]; break;
        case 'q': value = queenTable[actualRow][col]; break;
        case 'k': value = kingTable[actualRow][col]; break;
    }
    
    return isWhite ? value : -value;
}

bool ChessAI::isUnderAttack(const std::vector<std::vector<std::string>>& board, int row, int col, char attackingColor) {
    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            const std::string& piece = board[fromRow][fromCol];
            if (!piece.empty() && getPieceColor(piece) == attackingColor) {
                if (isValidMove(board, fromRow, fromCol, row, col)) {
                    return true;
                }
            }
        }
    }
    return false;
}

void ChessAI::orderMoves(std::vector<Move>& moves, const std::vector<std::vector<std::string>>& board) {
    // Simple move ordering: captures first, then other moves
    std::sort(moves.begin(), moves.end(), [&board](const Move& a, const Move& b) {
        bool aIsCapture = !board[a.toRow][a.toCol].empty();
        bool bIsCapture = !board[b.toRow][b.toCol].empty();
        
        if (aIsCapture && !bIsCapture) return true;
        if (!aIsCapture && bIsCapture) return false;
        
        if (aIsCapture && bIsCapture) {
            // Order captures by captured piece value
            int aValue = ChessAI().getPieceValue(board[a.toRow][a.toCol]);
            int bValue = ChessAI().getPieceValue(board[b.toRow][b.toCol]);
            return aValue > bValue;
        }
        
        return false; // Equal priority
    });
}
