// #include <QApplication>
// #include <QGridLayout>
// #include <QIcon>
// #include <QMouseEvent>
// #include <QObject>
// #include <QPushButton>
// #include <QWidget>
// #include <cctype>
// #include <iostream>
// #include <string>
// #include <vector>
//
// QPushButton *createImageButton(const QString &text, const QString &iconPath,
//                                const QSize &iconSize) {
//   QPushButton *button = new QPushButton(text);
//   button->setIcon(QIcon(iconPath));
//   button->setIconSize(iconSize);
//   return button;
// }
// int main(int argc, char *argv[]) {
//   QApplication app(argc, argv);
//   int height = 500;
//   int weight = 600;
//
//   // ChessBluetoothApplication window;
//   QWidget window;
//   window.resize(height, weight);
//   // Create a main window/widget
//   QGridLayout *grid = new QGridLayout(&window);
//
//   std::vector<std::vector<std::string>> board = {
//       {"r.png", "n.png", "b.png", "q.png", "k.png", "b.png", "n.png",
//       "r.png"},
//       {"p.png", "p.png", "p.png", "p.png", "p.png", "p.png", "p.png",
//       "p.png"},
//       {"", "", "", "", "", "", "", ""},
//       {"", "", "", "", "", "", "", ""},
//       {"", "", "", "", "", "", "", ""},
//       {"", "", "", "", "", "", "", ""},
//       {"P.png", "P.png", "P.png", "P.png", "P.png", "P.png", "P.png",
//       "P.png"},
//       {"R.png", "N.png", "B.png", "Q.png", "K.png", "B.png", "N.png",
//       "R.png"}};
//
//   // std::vector<std::vector<std::string>> board = {
//   //     {"RB.png", "NB.png", "BN.png", "QB.png", "KB.png", "BB.png",
//   "NB.png",
//   //     "RB.png"},
//   //     {"PB.png", "PB.png", "PB.png", "PB.png", "PB.png", "PB.png",
//   "PB.png",
//   //     "PB.png"},
//   //     {"", "", "", "", "", "", "", ""},
//   //     {"", "", "", "", "", "", "", ""},
//   //     {"", "", "", "", "", "", "", ""},
//   //     {"", "", "", "", "", "", "", ""},
//   //     {"PW.png", "PW.png", "PW.png", "PW.png", "PW.png", "PW.png",
//   "PW.png",
//   //     "PW.png"},
//   //     {"RW.png", "NW.png", "BW.png", "QW.png", "KW.png", "BW.png",
//   "NW.png",
//   //     "RW.png"}};
//
//   std::vector<std::vector<QPushButton *>> buttons(
//       8, std::vector<QPushButton *>(8));
//
//   bool IsAnyButtonsPressed = false;
//   int PreviousButtonLocationX = 0;
//   int PreviousButtonLocationY = 0;
//   std::string PieceType;
//   // std::string holder;
//
//   for (int x = 0; x < 8; x++) {
//     for (int y = 0; y < 8; y++) {
//
//       QPushButton *btn = new QPushButton();
//       btn->setFixedSize(window.height() / 8, window.width() / 8);
//
//       if (!board[x][y].empty()) {
//         btn->setIcon(QIcon(":/resources/Asset Images/" +
//                            QString::fromStdString(board[x][y])));
//         btn->setIconSize(btn->size());
//       }
//
//       if ((x + y) % 2 == 0)
//         btn->setStyleSheet("background-color: white;");
//       else
//         btn->setStyleSheet("background-color: gray;");
//
//       grid->addWidget(btn, x, y);
//       buttons[x][y] = btn;
//
//       QObject::connect(
//           btn, &QPushButton::clicked,
//           [&, x, y]() {
//             std::cout << "Button is pressed" << std::endl;
//             if (IsAnyButtonsPressed) {
//               if (!board[x][y].empty()) {
//                 if (isupper(board[x][y][0])) {
//                   PieceType = "WHITE";
//                 } else {
//                   PieceType = "BLACK";
//                 }
//                 // if (board[x][y][0] == 'p' || board[x][y][0] == 'P') {
//                 //   if (x == 1) {
//                 //     if () {
//                 //     }
//                 //   }
//               }
//             }
//           } else {
//             if (board[x][y][0] == 'p' || board[x][y][0] == 'P') {
//               if (x == 1
//               }
//               IsAnyButtonsPressed = true;
//               PreviousButtonLocationX = x;
//               PreviousButtonLocationY = y;
//             }
//           });
//     }
//   }
//
//   window.setLayout(grid);
//   window.show();
//   return app.exec();
// }

#include <QApplication>
#include <QGridLayout>
#include <QIcon>
#include <QPushButton>
#include <QWidget>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

bool isMoveValid(const std::vector<std::vector<std::string>> &board, int startX,
                 int startY, int endX, int endY) {
  if (startX == endX && startY == endY)
    return false; // Same square
  char piece = board[startX][startY][0];
  int dx = endX - startX;
  int dy = endY - startY;

  switch (tolower(piece)) {
  case 'p': {             // Pawn
    if (isupper(piece)) { // White
      if (dy == 0 && board[endX][endY].empty()) {
        if (dx == -1)
          return true;
        if (dx == -2 && startX == 6 && board[startX - 1][startY].empty())
          return true;
      }
      if (abs(dy) == 1 && dx == -1 && !board[endX][endY].empty() &&
          islower(board[endX][endY][0]))
        return true;
    } else { // Black
      if (dy == 0 && board[endX][endY].empty()) {
        if (dx == 1)
          return true;
        if (dx == 2 && startX == 1 && board[startX + 1][startY].empty())
          return true;
      }
      if (abs(dy) == 1 && dx == 1 && !board[endX][endY].empty() &&
          isupper(board[endX][endY][0]))
        return true;
    }
    return false;
  }
  case 'r': { // Rook
    if (dx != 0 && dy != 0)
      return false;
    int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
    int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);
    int x = startX + stepX, y = startY + stepY;
    while (x != endX || y != endY) {
      if (!board[x][y].empty())
        return false;
      x += stepX;
      y += stepY;
    }
    return true;
  }
  case 'n': // Knight
    return (abs(dx) == 2 && abs(dy) == 1) || (abs(dx) == 1 && abs(dy) == 2);
  case 'b': { // Bishop
    if (abs(dx) != abs(dy))
      return false;
    int stepX = dx > 0 ? 1 : -1;
    int stepY = dy > 0 ? 1 : -1;
    int x = startX + stepX, y = startY + stepY;
    while (x != endX || y != endY) {
      if (!board[x][y].empty())
        return false;
      x += stepX;
      y += stepY;
    }
    return true;
  }
  case 'q': { // Queen
    if (dx == 0 || dy == 0) {
      return isMoveValid(board, startX, startY, endX, endY); // Use rook logic
    }
    if (abs(dx) == abs(dy)) {
      return isMoveValid(board, startX, startY, endX, endY); // Use bishop logic
    }
    return false;
  }
  case 'k': // King
    return abs(dx) <= 1 && abs(dy) <= 1;
  }
  return false;
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QWidget window;
  window.resize(500, 600);
  QGridLayout *grid = new QGridLayout(&window);

  std::vector<std::vector<std::string>> board = {
      {"r.png", "n.png", "b.png", "q.png", "k.png", "b.png", "n.png", "r.png"},
      {"p.png", "p.png", "p.png", "p.png", "p.png", "p.png", "p.png", "p.png"},
      {"", "", "", "", "", "", "", ""},
      {"", "", "", "", "", "", "", ""},
      {"", "", "", "", "", "", "", ""},
      {"", "", "", "", "", "", "", ""},
      {"P.png", "P.png", "P.png", "P.png", "P.png", "P.png", "P.png", "P.png"},
      {"R.png", "N.png", "B.png", "Q.png", "K.png", "B.png", "N.png", "R.png"}};

  std::vector<std::vector<QPushButton *>> buttons(
      8, std::vector<QPushButton *>(8));
  bool pieceSelected = false;
  int selX = -1, selY = -1;

  auto refreshBoard = [&]() {
    for (int x = 0; x < 8; x++) {
      for (int y = 0; y < 8; y++) {
        if (!board[x][y].empty()) {
          buttons[x][y]->setIcon(QIcon(":/resources/Asset Images/" +
                                       QString::fromStdString(board[x][y])));
        } else {
          buttons[x][y]->setIcon(QIcon());
        }
      }
    }
  };

  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      QPushButton *btn = new QPushButton();
      btn->setFixedSize(window.height() / 8, window.width() / 8);

      if (!board[x][y].empty()) {
        btn->setIcon(QIcon(":/resources/Asset Images/" +
                           QString::fromStdString(board[x][y])));
        btn->setIconSize(btn->size());
      }

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
            std::cout << "Selected: " << board[x][y] << std::endl;
          }
        } else {
          if (isMoveValid(board, selX, selY, x, y)) {
            board[x][y] = board[selX][selY];
            board[selX][selY] = "";
            refreshBoard();
          }
          pieceSelected = false;
        }
      });
    }
  }

  window.setLayout(grid);
  window.show();
  return app.exec();
}
