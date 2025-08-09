#include <QApplication>
#include <QGridLayout>
#include <QIcon>
#include <QPushButton>
#include <QWidget>
#include <iostream>
#include <vector>
#include <string>
#include <cctype>

// Board representation: 8x8 array of piece codes:
// Uppercase = White, Lowercase = Black, empty = ""
// P/p = Pawn, R/r = Rook, N/n = Knight, B/b = Bishop, Q/q = Queen, K/k = King

using Board = std::vector<std::vector<std::string>>;

// Basic move validation for chess pieces
bool isMoveValid(const Board &board, int startX, int startY, int endX, int endY) {
    if (startX == endX && startY == endY) return false;
    if (endX < 0 || endX > 7 || endY < 0 || endY > 7) return false;

    const std::string &piece = board[startX][startY];
    if (piece.empty()) return false;

    char p = piece[0];
    bool isWhite = std::isupper(p);
    int dx = endX - startX;
    int dy = endY - startY;

    // Prevent capturing your own piece
    if (!board[endX][endY].empty()) {
        bool targetIsWhite = std::isupper(board[endX][endY][0]);
        if (targetIsWhite == isWhite) return false;
    }

    switch (tolower(p)) {
        case 'p': {
            int dir = isWhite ? -1 : 1;
            int startRow = isWhite ? 6 : 1;
            // Normal move forward
            if (dy == 0 && dx == dir && board[endX][endY].empty()) return true;
            // Initial two-square move
            if (dy == 0 && dx == 2*dir && startX == startRow &&
                board[startX + dir][startY].empty() && board[endX][endY].empty())
                return true;
            // Captures
            if (abs(dy) == 1 && dx == dir && !board[endX][endY].empty()) return true;
            return false;
        }
        case 'r': {
            if (dx != 0 && dy != 0) return false;
            int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
            int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
            int x = startX + stepX, y = startY + stepY;
            while (x != endX || y != endY) {
                if (!board[x][y].empty()) return false;
                x += stepX;
                y += stepY;
            }
            return true;
        }
        case 'n': {
            return (abs(dx) == 2 && abs(dy) == 1) || (abs(dx) == 1 && abs(dy) == 2);
        }
        case 'b': {
            if (abs(dx) != abs(dy)) return false;
            int stepX = (dx > 0) ? 1 : -1;
            int stepY = (dy > 0) ? 1 : -1;
            int x = startX + stepX, y = startY + stepY;
            while (x != endX && y != endY) {
                if (!board[x][y].empty()) return false;
                x += stepX;
                y += stepY;
            }
            return true;
        }
        case 'q': {
            // Queen combines rook + bishop moves
            if (dx == 0 || dy == 0) {
                // Rook-like move
                int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
                int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
                int x = startX + stepX, y = startY + stepY;
                while (x != endX || y != endY) {
                    if (!board[x][y].empty()) return false;
                    x += stepX;
                    y += stepY;
                }
                return true;
            } else if (abs(dx) == abs(dy)) {
                // Bishop-like move
                int stepX = (dx > 0) ? 1 : -1;
                int stepY = (dy > 0) ? 1 : -1;
                int x = startX + stepX, y = startY + stepY;
                while (x != endX && y != endY) {
                    if (!board[x][y].empty()) return false;
                    x += stepX;
                    y += stepY;
                }
                return true;
            }
            return false;
        }
        case 'k': {
            return abs(dx) <= 1 && abs(dy) <= 1;
        }
    }
    return false;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.resize(600, 600);
    QGridLayout *grid = new QGridLayout(&window);
    grid->setSpacing(0);

    Board board = {
        {"r", "n", "b", "q", "k", "b", "n", "r"},
        {"p", "p", "p", "p", "p", "p", "p", "p"},
        {"",  "",  "",  "",  "",  "",  "",  ""},
        {"",  "",  "",  "",  "",  "",  "",  ""},
        {"",  "",  "",  "",  "",  "",  "",  ""},
        {"",  "",  "",  "",  "",  "",  "",  ""},
        {"P", "P", "P", "P", "P", "P", "P", "P"},
        {"R", "N", "B", "Q", "K", "B", "N", "R"}
    };

    std::vector<std::vector<QPushButton*>> buttons(8, std::vector<QPushButton*>(8));
    bool pieceSelected = false;
    int selX = -1, selY = -1;

    auto updateButtonIcon = [&](int x, int y) {
        if (board[x][y].empty()) {
            buttons[x][y]->setIcon(QIcon());
        } else {
            // Assuming you have piece icons named e.g. "P.png", "k.png", etc.
            QString iconPath = QString(":/resources/Asset Images/") + QString::fromStdString(board[x][y]) + ".png";
            buttons[x][y]->setIcon(QIcon(iconPath));
            buttons[x][y]->setIconSize(buttons[x][y]->size());
        }
    };

    auto refreshBoardUI = [&]() {
        for (int x = 0; x < 8; ++x)
            for (int y = 0; y < 8; ++y)
                updateButtonIcon(x, y);
    };

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            QPushButton *btn = new QPushButton();
            btn->setFixedSize(75, 75);

            if ((x + y) % 2 == 0)
                btn->setStyleSheet("background-color: white;");
            else
                btn->setStyleSheet("background-color: gray;");

            grid->addWidget(btn, x, y);
            buttons[x][y] = btn;

            QObject::connect(btn, &QPushButton::clicked, [&, x, y]() {
                if (!pieceSelected) {
                    if (!board[x][y].empty()) {
                        pieceSelected = true;
                        selX = x;
                        selY = y;
                        buttons[x][y]->setStyleSheet("background-color: yellow;");  // highlight selected
                    }
                } else {
                    if (x == selX && y == selY) {
                        // Deselect
                        pieceSelected = false;
                        buttons[x][y]->setStyleSheet((x + y) % 2 == 0 ? "background-color: white;" : "background-color: gray;");
                    } else if (isMoveValid(board, selX, selY, x, y)) {
                        // Move piece
                        board[x][y] = board[selX][selY];
                        board[selX][selY] = "";
                        pieceSelected = false;
                        // Reset style of previously selected button
                        buttons[selX][selY]->setStyleSheet((selX + selY) % 2 == 0 ? "background-color: white;" : "background-color: gray;");
                        refreshBoardUI();
                    } else {
                        // Invalid move, deselect
                        pieceSelected = false;
                        buttons[selX][selY]->setStyleSheet((selX + selY) % 2 == 0 ? "background-color: white;" : "background-color: gray;");
                    }
                }
            });
        }
    }

    refreshBoardUI();

    window.setLayout(grid);
    window.show();

    return app.exec();
}
