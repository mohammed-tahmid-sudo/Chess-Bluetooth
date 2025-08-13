#include "chessboard.h"
#include <QPaintEvent>
#include <QDebug>
#include <cctype>
#include <cmath>

ChessBoard::ChessBoard(QWidget *parent)
    : QWidget(parent), selectedRow(-1), selectedCol(-1), pieceSelected(false), currentPlayer('w'),
      aiEnabled(false), aiColor('b'), chessAI(nullptr), aiTimer(nullptr)
{
    setFixedSize(BOARD_SIZE * SQUARE_SIZE, BOARD_SIZE * SQUARE_SIZE);
    setMouseTracking(true);
    loadPieceImages();
    initializeBoard();
    
    // Initialize AI
    chessAI = new ChessAI(ChessAI::MEDIUM);
    
    // Initialize AI timer
    aiTimer = new QTimer(this);
    aiTimer->setSingleShot(true);
    connect(aiTimer, &QTimer::timeout, this, &ChessBoard::onAITimerTimeout);
}

ChessBoard::~ChessBoard()
{
    delete chessAI;
}

void ChessBoard::loadPieceImages()
{
    // Load piece images from resources
    pieceImages["r"] = QPixmap(":/RB.png").scaled(SQUARE_SIZE - 10, SQUARE_SIZE - 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pieceImages["n"] = QPixmap(":/NB.png").scaled(SQUARE_SIZE - 10, SQUARE_SIZE - 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pieceImages["b"] = QPixmap(":/BB.png").scaled(SQUARE_SIZE - 10, SQUARE_SIZE - 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pieceImages["q"] = QPixmap(":/QB.png").scaled(SQUARE_SIZE - 10, SQUARE_SIZE - 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pieceImages["k"] = QPixmap(":/KB.png").scaled(SQUARE_SIZE - 10, SQUARE_SIZE - 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pieceImages["p"] = QPixmap(":/PB.png").scaled(SQUARE_SIZE - 10, SQUARE_SIZE - 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    pieceImages["R"] = QPixmap(":/RW.png").scaled(SQUARE_SIZE - 10, SQUARE_SIZE - 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pieceImages["N"] = QPixmap(":/NW.png").scaled(SQUARE_SIZE - 10, SQUARE_SIZE - 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pieceImages["B"] = QPixmap(":/BW.png").scaled(SQUARE_SIZE - 10, SQUARE_SIZE - 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pieceImages["Q"] = QPixmap(":/QW.png").scaled(SQUARE_SIZE - 10, SQUARE_SIZE - 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pieceImages["K"] = QPixmap(":/KW.png").scaled(SQUARE_SIZE - 10, SQUARE_SIZE - 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pieceImages["P"] = QPixmap(":/PW.png").scaled(SQUARE_SIZE - 10, SQUARE_SIZE - 10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void ChessBoard::initializeBoard()
{
    board = {
        {"r", "n", "b", "q", "k", "b", "n", "r"}, // 0 black back rank
        {"p", "p", "p", "p", "p", "p", "p", "p"}, // 1 black pawns
        {"", "", "", "", "", "", "", ""},         // 2
        {"", "", "", "", "", "", "", ""},         // 3
        {"", "", "", "", "", "", "", ""},         // 4
        {"", "", "", "", "", "", "", ""},         // 5
        {"P", "P", "P", "P", "P", "P", "P", "P"}, // 6 white pawns
        {"R", "N", "B", "Q", "K", "B", "N", "R"}  // 7 white back rank
    };
}

void ChessBoard::resetBoard()
{
    initializeBoard();
    currentPlayer = 'w';
    pieceSelected = false;
    selectedRow = selectedCol = -1;
    update();
    emit gameStatusChanged("New game started - White to move");
    
    // Check for AI turn at game start
    checkForAITurn();
}

void ChessBoard::paintEvent(QPaintEvent */*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    drawBoard(painter);
    drawSelection(painter);
    drawPieces(painter);
}

void ChessBoard::drawBoard(QPainter &painter)
{
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            QRect square(col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
            
            // Alternate colors for chess board
            if ((row + col) % 2 == 0) {
                painter.fillRect(square, QColor(240, 217, 181)); // Light squares
            } else {
                painter.fillRect(square, QColor(181, 136, 99));  // Dark squares
            }
            
            // Draw square border
            painter.setPen(QPen(Qt::black, 1));
            painter.drawRect(square);
        }
    }
}

void ChessBoard::drawSelection(QPainter &painter)
{
    if (pieceSelected && selectedRow >= 0 && selectedCol >= 0) {
        QRect square(selectedCol * SQUARE_SIZE, selectedRow * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
        painter.setPen(QPen(Qt::yellow, 4));
        painter.drawRect(square);
    }
}

void ChessBoard::drawPieces(QPainter &painter)
{
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            const std::string &piece = board[row][col];
            if (!piece.empty() && pieceImages.find(piece) != pieceImages.end()) {
                QRect square(col * SQUARE_SIZE + 5, row * SQUARE_SIZE + 5, 
                           SQUARE_SIZE - 10, SQUARE_SIZE - 10);
                painter.drawPixmap(square, pieceImages[piece]);
            }
        }
    }
}

void ChessBoard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) return;
    
    // Prevent human input if it's AI's turn
    if (aiEnabled && currentPlayer == aiColor) {
        return;
    }
    
    int col = static_cast<int>(event->position().x()) / SQUARE_SIZE;
    int row = static_cast<int>(event->position().y()) / SQUARE_SIZE;
    
    // Check bounds
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return;
    
    if (!pieceSelected) {
        // Select a piece
        if (!board[row][col].empty()) {
            char pieceRace = getPieceRace(board[row][col]);
            if (pieceRace == currentPlayer) {
                selectedRow = row;
                selectedCol = col;
                pieceSelected = true;
                update();
            }
        }
    } else {
        // Try to move the selected piece
        if (row == selectedRow && col == selectedCol) {
            // Deselect if clicking same square
            pieceSelected = false;
            selectedRow = selectedCol = -1;
        } else if (isValidMove(selectedRow, selectedCol, row, col)) {
            QString moveNotation = getMoveNotation(selectedRow, selectedCol, row, col);
            makeMove(selectedRow, selectedCol, row, col);
            emit moveMade(moveNotation);
            emit moveMadeWithCoords(moveNotation, selectedRow, selectedCol, row, col);
            pieceSelected = false;
            selectedRow = selectedCol = -1;
            switchPlayer();
        } else {
            // Invalid move - try to select a new piece if it belongs to current player
            if (!board[row][col].empty()) {
                char pieceRace = getPieceRace(board[row][col]);
                if (pieceRace == currentPlayer) {
                    selectedRow = row;
                    selectedCol = col;
                } else {
                    pieceSelected = false;
                    selectedRow = selectedCol = -1;
                }
            } else {
                pieceSelected = false;
                selectedRow = selectedCol = -1;
            }
        }
        update();
    }
}

bool ChessBoard::isValidMove(int fromRow, int fromCol, int toRow, int toCol)
{
    if (board[fromRow][fromCol].empty()) return false;
    
    std::string piece = board[fromRow][fromCol];
    char race = getPieceRace(piece);
    char name = std::tolower(piece[0]);
    
    return CheckIfValidMove(fromRow, fromCol, toRow, toCol, board, race, name);
}

void ChessBoard::makeMove(int fromRow, int fromCol, int toRow, int toCol)
{
    board[toRow][toCol] = board[fromRow][fromCol];
    board[fromRow][fromCol] = "";
    update();
}

char ChessBoard::getPieceRace(const std::string &piece)
{
    if (piece.empty()) return 'n';
    return std::isupper(piece[0]) ? 'w' : 'b';
}

void ChessBoard::switchPlayer()
{
    currentPlayer = (currentPlayer == 'w') ? 'b' : 'w';
    QString playerName = (currentPlayer == 'w') ? "White" : "Black";
    emit gameStatusChanged(playerName + " to move");
    
    // Check for AI turn
    checkForAITurn();
}

QString ChessBoard::getMoveNotation(int fromRow, int fromCol, int toRow, int toCol)
{
    QString from = QString("%1%2").arg(char('a' + fromCol)).arg(8 - fromRow);
    QString to = QString("%1%2").arg(char('a' + toCol)).arg(8 - toRow);
    QString piece = QString::fromStdString(board[fromRow][fromCol]);
    
    return QString("%1: %2-%3").arg(piece, from, to);
}

// Chess logic functions (from original main.cpp)
char Race(std::string str) {
    if (str.empty()) return 'n'; // no piece
    for (char c : str) {
        if (!isupper(static_cast<unsigned char>(c))) {
            return 'b';  // Found a lowercase character (black piece)
        }
    }
    return 'w'; // All uppercase (white piece)
}

bool CheckIfValidMove(int CurrentPosX, int CurrentPosY, int ExpectedPosX,
                      int ExpectedPosY,
                      std::vector<std::vector<std::string>> Board, char Race,
                      char name) {
    // Bounds checking
    if (CurrentPosX < 0 || CurrentPosX > 7 || CurrentPosY < 0 || CurrentPosY > 7 ||
        ExpectedPosX < 0 || ExpectedPosX > 7 || ExpectedPosY < 0 || ExpectedPosY > 7) {
        return false; // Out of bounds
    }
    
    // Can't move to same position
    if (CurrentPosX == ExpectedPosX && CurrentPosY == ExpectedPosY) {
        return false;
    }
    
    // Can't capture own pieces
    if (!Board[ExpectedPosX][ExpectedPosY].empty()) {
        char targetRace = ::Race(Board[ExpectedPosX][ExpectedPosY]);
        if (targetRace == Race) {
            return false;
        }
    }
    
    char opposite = (Race == 'b') ? 'w' : 'b';

    if (name == 'p' || name == 'P') {
        if (Race == 'w') {
            // single forward
            if (ExpectedPosX == CurrentPosX - 1 && ExpectedPosY == CurrentPosY &&
                Board[ExpectedPosX][ExpectedPosY].empty())
                return true;

            // double forward from start
            if (CurrentPosX == 6 && ExpectedPosX == CurrentPosX - 2 &&
                ExpectedPosY == CurrentPosY &&
                Board[CurrentPosX - 1][CurrentPosY].empty() &&
                Board[ExpectedPosX][ExpectedPosY].empty())
                return true;

            // capture
            if (ExpectedPosX == CurrentPosX - 1 &&
                (ExpectedPosY == CurrentPosY - 1 ||
                 ExpectedPosY == CurrentPosY + 1) &&
                !Board[ExpectedPosX][ExpectedPosY].empty() &&
                ::Race(Board[ExpectedPosX][ExpectedPosY]) == opposite)
                return true;
        }
        if (Race == 'b') {
            if (ExpectedPosX == CurrentPosX + 1 && ExpectedPosY == CurrentPosY &&
                Board[ExpectedPosX][ExpectedPosY].empty())
                return true;

            // double forward from start
            if (CurrentPosX == 1 && ExpectedPosX == CurrentPosX + 2 &&
                ExpectedPosY == CurrentPosY &&
                Board[CurrentPosX + 1][CurrentPosY].empty() &&
                Board[ExpectedPosX][ExpectedPosY].empty())
                return true;

            // capture
            if (ExpectedPosX == CurrentPosX + 1 &&
                (ExpectedPosY == CurrentPosY - 1 ||
                 ExpectedPosY == CurrentPosY + 1) &&
                !Board[ExpectedPosX][ExpectedPosY].empty() &&
                ::Race(Board[ExpectedPosX][ExpectedPosY]) == opposite)
                return true;
        }
    } else if (name == 'r' || name == 'R') {
        if (CurrentPosX == ExpectedPosX) {
            // doing for the Y value
            int y = (ExpectedPosY > CurrentPosY) ? 1 : -1;

            for (int i = CurrentPosY + y; i != ExpectedPosY; i += y) {
                if (!Board[CurrentPosX][i].empty()) {
                    return false;
                }
            }
            return Board[ExpectedPosX][ExpectedPosY].empty() ||
                   ::Race(Board[ExpectedPosX][ExpectedPosY]) == opposite;

        } else if (CurrentPosY == ExpectedPosY) {

            int x = (ExpectedPosX > CurrentPosX) ? 1 : -1;

            for (int i = CurrentPosX + x; i != ExpectedPosX; i += x) {
                if (!Board[i][CurrentPosY].empty()) {
                    return false;
                }
            }

            return Board[ExpectedPosX][ExpectedPosY].empty() ||
                   ::Race(Board[ExpectedPosX][ExpectedPosY]) == opposite;
        }
    } else if (name == 'n' || name == 'N') {
        int dx = std::abs(ExpectedPosX - CurrentPosX);
        int dy = std::abs(ExpectedPosY - CurrentPosY);
        if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2)) {
            return Board[ExpectedPosX][ExpectedPosY].empty() ||
                   ::Race(Board[ExpectedPosX][ExpectedPosY]) == opposite;
        }
    } else if (name == 'b' || name == 'B') {
        // Bishop
        if (abs(ExpectedPosX - CurrentPosX) != abs(ExpectedPosY - CurrentPosY))
            return false;

        int dx = (ExpectedPosX > CurrentPosX) ? 1 : -1;
        int dy = (ExpectedPosY > CurrentPosY) ? 1 : -1;

        int x = CurrentPosX + dx;
        int y = CurrentPosY + dy;

        while (x != ExpectedPosX && y != ExpectedPosY) {
            if (!Board[x][y].empty())
                return false;
            x += dx;
            y += dy;
        }

        return Board[ExpectedPosX][ExpectedPosY].empty() ||
               ::Race(Board[ExpectedPosX][ExpectedPosY]) == opposite;
    } else if (name == 'k' || name == 'K') {
        // King - can move one square in any direction
        int dx = std::abs(ExpectedPosX - CurrentPosX);
        int dy = std::abs(ExpectedPosY - CurrentPosY);
        if ((dx == 0 && dy == 1) || (dx == 1 && dy == 0) || (dx == 1 && dy == 1)) {
            return Board[ExpectedPosX][ExpectedPosY].empty() ||
                   ::Race(Board[ExpectedPosX][ExpectedPosY]) == opposite;
        }
    } else if (name == 'q' || name == 'Q') {
        int dx = ExpectedPosX - CurrentPosX;
        int dy = ExpectedPosY - CurrentPosY;

        if (dx == 0 || dy == 0) {
            // Rook-like move
            int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
            int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

            int x = CurrentPosX + stepX;
            int y = CurrentPosY + stepY;

            while (x != ExpectedPosX || y != ExpectedPosY) {
                if (!Board[x][y].empty())
                    return false;
                x += stepX;
                y += stepY;
            }
        } else if (abs(dx) == abs(dy)) {
            // Bishop-like move
            int stepX = (dx > 0) ? 1 : -1;
            int stepY = (dy > 0) ? 1 : -1;

            int x = CurrentPosX + stepX;
            int y = CurrentPosY + stepY;

            while (x != ExpectedPosX && y != ExpectedPosY) {
                if (!Board[x][y].empty())
                    return false;
                x += stepX;
                y += stepY;
            }
        } else {
            return false; // invalid queen move
        }

        return Board[ExpectedPosX][ExpectedPosY].empty() ||
               ::Race(Board[ExpectedPosX][ExpectedPosY]) == opposite;
    }

    return false;
}

// AI Implementation
void ChessBoard::setAIEnabled(bool enabled) {
    aiEnabled = enabled;
    if (enabled) {
        checkForAITurn();
    }
}

void ChessBoard::setAIColor(char color) {
    aiColor = color;
}

void ChessBoard::setAIDifficulty(ChessAI::Difficulty difficulty) {
    if (chessAI) {
        chessAI->setDifficulty(difficulty);
    }
}

void ChessBoard::makeAIMove() {
    if (!chessAI || !aiEnabled || currentPlayer != aiColor) {
        return;
    }
    
    Move aiMove = chessAI->getBestMove(board, aiColor);
    
    if (aiMove.fromRow != -1) { // Valid move found
        QString moveNotation = getMoveNotation(aiMove.fromRow, aiMove.fromCol, 
                                             aiMove.toRow, aiMove.toCol);
        makeMove(aiMove.fromRow, aiMove.fromCol, aiMove.toRow, aiMove.toCol);
        emit moveMade(moveNotation);
        switchPlayer();
        
        // Don't check for AI turn recursively - let switchPlayer handle it
    }
}

void ChessBoard::checkForAITurn() {
    if (aiEnabled && currentPlayer == aiColor) {
        QString aiColorName = (aiColor == 'w') ? "White AI" : "Black AI";
        emit gameStatusChanged(aiColorName + " is thinking...");
        
        // Add a small delay to make AI moves visible (1 second)
        aiTimer->start(1000);
    }
}

void ChessBoard::onAITimerTimeout() {
    makeAIMove();
}

// External move handling for Bluetooth multiplayer
bool ChessBoard::makeExternalMove(int fromRow, int fromCol, int toRow, int toCol) {
    // Validate move
    if (!isValidMove(fromRow, fromCol, toRow, toCol)) {
        return false;
    }
    
    // Make the move
    QString moveNotation = getMoveNotation(fromRow, fromCol, toRow, toCol);
    makeMove(fromRow, fromCol, toRow, toCol);
    emit moveMade(moveNotation);
    switchPlayer();
    
    return true;
}
