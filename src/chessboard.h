#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QPixmap>
#include <QTimer>
#include <vector>
#include <string>
#include <map>
#include "chessai.h"

class ChessBoard : public QWidget
{
    Q_OBJECT

public:
    explicit ChessBoard(QWidget *parent = nullptr);
    ~ChessBoard();
    void resetBoard();
    
    // AI-related methods
    void setAIEnabled(bool enabled);
    void setAIColor(char color); // 'w' for white AI, 'b' for black AI
    void setAIDifficulty(ChessAI::Difficulty difficulty);
    bool isAIEnabled() const { return aiEnabled; }
    char getAIColor() const { return aiColor; }
    
    // External move handling (for Bluetooth multiplayer)
    bool makeExternalMove(int fromRow, int fromCol, int toRow, int toCol);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    
private:
    static const int BOARD_SIZE = 8;
    static const int SQUARE_SIZE = 80;
    
    std::vector<std::vector<std::string>> board;
    std::map<std::string, QPixmap> pieceImages;
    
    int selectedRow;
    int selectedCol;
    bool pieceSelected;
    char currentPlayer; // 'w' for white, 'b' for black
    
    // AI-related members
    bool aiEnabled;
    char aiColor;
    ChessAI* chessAI;
    QTimer* aiTimer;
    
    void loadPieceImages();
    void initializeBoard();
    void drawBoard(QPainter &painter);
    void drawPieces(QPainter &painter);
    void drawSelection(QPainter &painter);
    void makeMove(int fromRow, int fromCol, int toRow, int toCol);
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol);
    char getPieceRace(const std::string &piece);
    void switchPlayer();
    QString getMoveNotation(int fromRow, int fromCol, int toRow, int toCol);
    
    // AI-related private methods
    void makeAIMove();
    void checkForAITurn();
    
private slots:
    void onAITimerTimeout();
    
signals:
    void moveMade(QString moveNotation);
    void moveMadeWithCoords(QString moveNotation, int fromRow, int fromCol, int toRow, int toCol);
    void gameStatusChanged(QString status);
};

// Chess logic functions (from original code)
char Race(std::string str);
bool CheckIfValidMove(int CurrentPosX, int CurrentPosY, int ExpectedPosX,
                      int ExpectedPosY,
                      std::vector<std::vector<std::string>> Board, char Race,
                      char name);

#endif // CHESSBOARD_H
