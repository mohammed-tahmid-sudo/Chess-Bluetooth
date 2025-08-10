#include <QApplication>
#include <QMainWindow>
#include <QPainter>
#include <QMouseEvent>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cmath>

bool CheckIfValidMove(int CurrentPosX, int CurrentPosY, int ExpectedPosX,
                      int ExpectedPosY,
                      std::vector<std::vector<std::string>> Board, char Race,
                      char name) {
  char opposite = (Race == 'b') ? 'w' : 'b';

  if (name == 'p' || name == 'P') {
    if (Race == 'w') {
      if (CurrentPosX == 1) {
        if (ExpectedPosX == CurrentPosX + 1 &&
            Board[ExpectedPosX][ExpectedPosY].empty()) {
          return true;
        }

        if (ExpectedPosX == CurrentPosX + 1 &&
            Board[ExpectedPosX][ExpectedPosY].empty()) {
          return true;
        }

        if (ExpectedPosX == CurrentPosX + 1 &&
            ExpectedPosY == CurrentPosY + 1 &&
            !Board[ExpectedPosX][ExpectedPosY].empty() &&
            Board[ExpectedPosX][ExpectedPosY][0] != opposite) {
          // Permition Denied
          return false;
        }
      }
    }
    if (Race == 'b') {

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

      // capture left/right
      if (ExpectedPosX == CurrentPosX - 1 &&
          (ExpectedPosY == CurrentPosY - 1 ||
           ExpectedPosY == CurrentPosY + 1) &&
          !Board[ExpectedPosX][ExpectedPosY].empty() &&
          Board[ExpectedPosX][ExpectedPosY][0] == opposite)
        return true;
    }
  } else if (name == 'r' || name == 'R') { // Rook
    if (CurrentPosX == ExpectedPosX || CurrentPosY == ExpectedPosY) {
      // Check path clear
      int xDir = (ExpectedPosX > CurrentPosX)   ? 1
                 : (ExpectedPosX < CurrentPosX) ? -1
                                                : 0;
      int yDir = (ExpectedPosY > CurrentPosY)   ? 1
                 : (ExpectedPosY < CurrentPosY) ? -1
                                                : 0;

      int x = CurrentPosX + xDir;
      int y = CurrentPosY + yDir;
      while (x != ExpectedPosX || y != ExpectedPosY) {
        if (!Board[x][y].empty())
          return false;
        x += xDir;
        y += yDir;
      }
      // Destination must be empty or enemy
      if (Board[ExpectedPosX][ExpectedPosY].empty() ||
          Board[ExpectedPosX][ExpectedPosY][0] == opposite)
        return true;
    }
  } else if (name == 'n' || name == 'N') { // Knight
    int dx = abs(ExpectedPosX - CurrentPosX);
    int dy = abs(ExpectedPosY - CurrentPosY);
    if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2)) {
      if (Board[ExpectedPosX][ExpectedPosY].empty() ||
          Board[ExpectedPosX][ExpectedPosY][0] == opposite)
        return true;
    }
  } else if (name == 'b' || name == 'B') { // Bishop
    int dx = ExpectedPosX - CurrentPosX;
    int dy = ExpectedPosY - CurrentPosY;
    if (abs(dx) == abs(dy)) {
      int xDir = (dx > 0) ? 1 : -1;
      int yDir = (dy > 0) ? 1 : -1;
      int x = CurrentPosX + xDir;
      int y = CurrentPosY + yDir;
      while (x != ExpectedPosX && y != ExpectedPosY) {
        if (!Board[x][y].empty())
          return false;
        x += xDir;
        y += yDir;
      }
      if (Board[ExpectedPosX][ExpectedPosY].empty() ||
          Board[ExpectedPosX][ExpectedPosY][0] == opposite)
        return true;
    }
  } else if (name == 'q' || name == 'Q') { // Queen
    int dx = ExpectedPosX - CurrentPosX;
    int dy = ExpectedPosY - CurrentPosY;

    // Rook-like move
    if (CurrentPosX == ExpectedPosX || CurrentPosY == ExpectedPosY) {
      int xDir = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
      int yDir = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

      int x = CurrentPosX + xDir;
      int y = CurrentPosY + yDir;
      while (x != ExpectedPosX || y != ExpectedPosY) {
        if (!Board[x][y].empty())
          return false;
        x += xDir;
        y += yDir;
      }
      if (Board[ExpectedPosX][ExpectedPosY].empty() ||
          Board[ExpectedPosX][ExpectedPosY][0] == opposite)
        return true;
    }
    // Bishop-like move
    else if (abs(dx) == abs(dy)) {
      int xDir = (dx > 0) ? 1 : -1;
      int yDir = (dy > 0) ? 1 : -1;

      int x = CurrentPosX + xDir;
      int y = CurrentPosY + yDir;
      while (x != ExpectedPosX && y != ExpectedPosY) {
        if (!Board[x][y].empty())
          return false;
        x += xDir;
        y += yDir;
      }
      if (Board[ExpectedPosX][ExpectedPosY].empty() ||
          Board[ExpectedPosX][ExpectedPosY][0] == opposite)
        return true;
    }
  } else if (name == 'k' || name == 'K') { // King
    int dx = abs(ExpectedPosX - CurrentPosX);
    int dy = abs(ExpectedPosY - CurrentPosY);
    if ((dx <= 1 && dy <= 1) &&
        (Board[ExpectedPosX][ExpectedPosY].empty() ||
         Board[ExpectedPosX][ExpectedPosY][0] == opposite))
      return true;
  }

  return 0;
}

// int main(int argc, char *argv[]) {
//   std::vector<std::vector<std::string>> board = {
//       {"bR", "bN", "bB", "bQ", "bK", "bB", "bN", "bR"},
//       {"bP", "bP", "bP", "bP", "bP", "bP", "bP", "bP"},
//       {"", "", "", "", "", "", "", ""},
//       {"", "", "", "", "", "", "", ""},
//       {"", "", "", "", "", "", "", ""},
//       {"", "", "", "", "", "", "", ""},
//       {"wP", "wP", "wP", "wP", "wP", "wP", "wP", "wP"},
//       {"wR", "wN", "wB", "wQ", "wK", "wB", "wN", "wR"}};
//
//   QApplication app(argc, argv);
//
//   QMainWindow mainWindow;
//
//   // Get the current palette of the main window
//   QPalette palette = mainWindow.palette();
//
//   // Set the Window role color to black
//   palette.setColor(QPalette::Window, Qt::white);
//
//   // Apply the modified palette to the main window
//   mainWindow.setPalette(palette);
//
//   // Ensure the background is filled
//   mainWindow.setAutoFillBackground(true);
//
//   mainWindow.setWindowTitle("Black Window");
//   mainWindow.show();
//
//   return app.exec();
// }
// --- ChessBoard widget ---
class ChessBoard : public QWidget {
  Q_OBJECT

public:
  ChessBoard(QWidget *parent = nullptr)
      : QWidget(parent), squareSize(80), selectedRow(-1), selectedCol(-1),
        turn('w') {
    setMinimumSize(8 * squareSize, 8 * squareSize);

    board = {
        {"bR", "bN", "bB", "bQ", "bK", "bB", "bN", "bR"},
        {"bP", "bP", "bP", "bP", "bP", "bP", "bP", "bP"},
        {"", "", "", "", "", "", "", ""},
        {"", "", "", "", "", "", "", ""},
        {"", "", "", "", "", "", "", ""},
        {"", "", "", "", "", "", "", ""},
        {"wP", "wP", "wP", "wP", "wP", "wP", "wP", "wP"},
        {"wR", "wN", "wB", "wQ", "wK", "wB", "wN", "wR"}};
  }

  void paintEvent(QPaintEvent *) override {
    QPainter p(this);
    QFont f = p.font();
    f.setPointSize(32);
    p.setFont(f);

    for (int r = 0; r < 8; ++r) {
      for (int c = 0; c < 8; ++c) {
        QRect rect(c * squareSize, r * squareSize, squareSize, squareSize);
        bool light = ((r + c) % 2 == 0);
        p.fillRect(rect, light ? QColor(240, 217, 181) : QColor(181, 136, 99));

        // highlight selection
        if (r == selectedRow && c == selectedCol) {
          p.setBrush(QBrush(QColor(200, 200, 0, 120)));
          p.drawRect(rect);
          p.setBrush(Qt::NoBrush);
        }

        // draw piece (unicode)
        if (!board[r][c].empty()) {
          char color = board[r][c][0];
          char piece = board[r][c][1];

          QString glyph = pieceToUnicode(color, piece);
          p.setPen((color == 'w') ? Qt::white : Qt::black);

          // Draw with slight shadow/outline for readability
          p.drawText(rect, Qt::AlignCenter, glyph);
        }
      }
    }
  }

  void mousePressEvent(QMouseEvent *event) override {
    int col = event->position().x() / squareSize;
    int row = event->position().y() / squareSize;
    if (col < 0 || col > 7 || row < 0 || row > 7)
      return;

    // first click: select
    if (selectedRow == -1) {
      if (!board[row][col].empty() && board[row][col][0] == turn) {
        selectedRow = row;
        selectedCol = col;
        update();
      }
      // else ignore
    } else {
      // attempt move
      std::string fromPiece = board[selectedRow][selectedCol];
      if (!fromPiece.empty()) {
        char race = fromPiece[0];
        char name = std::tolower(fromPiece[1]);

        bool ok = CheckIfValidMove(selectedRow, selectedCol, row, col, board, race, name);

        // allow capture or move only if validation passes and destination not same-color
        if (ok) {
          // destination same color check (should be handled in validation but double-check)
          if (!board[row][col].empty() && board[row][col][0] == race) {
            // invalid
          } else {
            board[row][col] = board[selectedRow][selectedCol];
            board[selectedRow][selectedCol].clear();

            // simple promotion (pawn reaches last rank)
            if ((board[row][col][1] == 'P' || board[row][col][1] == 'p') &&
                (row == 0 || row == 7)) {
              board[row][col][1] = 'Q'; // promote to queen
            }

            // toggle turn
            turn = (turn == 'w') ? 'b' : 'w';
          }
        }
      }

      // clear selection always
      selectedRow = selectedCol = -1;
      update();
      emit turnChanged(turn);
    }
  }

  char currentTurn() const { return turn; }

signals:
  void turnChanged(char newTurn);

private:
  int squareSize;
  int selectedRow, selectedCol;
  char turn;
  std::vector<std::vector<std::string>> board;

  QString pieceToUnicode(char color, char piece) {
    // Unicode chess pieces
    // White: ♔♕♖♗♘♙  (U+2654..)
    // Black: ♚♛♜♝♞♟
    static std::map<char, QChar> white = {
        {'k', QChar(0x2654)}, {'q', QChar(0x2655)}, {'r', QChar(0x2656)},
        {'b', QChar(0x2657)}, {'n', QChar(0x2658)}, {'p', QChar(0x2659)}};
    static std::map<char, QChar> black = {
        {'k', QChar(0x265A)}, {'q', QChar(0x265B)}, {'r', QChar(0x265C)},
        {'b', QChar(0x265D)}, {'n', QChar(0x265E)}, {'p', QChar(0x265F)}};

    char lower = std::tolower(piece);
    if (color == 'w') {
      return QString(white.count(lower) ? white[lower] : QChar('?'));
    } else {
      return QString(black.count(lower) ? black[lower] : QChar('?'));
    }
  }
};

// --- main window and UI ---
int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QMainWindow mainWindow;
  QWidget *central = new QWidget;
  QVBoxLayout *layout = new QVBoxLayout;

  ChessBoard *boardWidget = new ChessBoard;
  QLabel *status = new QLabel(QString("Turn: White"));

  QObject::connect(boardWidget, &ChessBoard::turnChanged, [&](char t) {
    status->setText(QString("Turn: %1").arg(t == 'w' ? "White" : "Black"));
  });

  layout->addWidget(boardWidget);
  layout->addWidget(status);
  central->setLayout(layout);

  mainWindow.setCentralWidget(central);
  mainWindow.setWindowTitle("Qt Chess");
  mainWindow.show();

  return app.exec();
}

#include "main.moc"
