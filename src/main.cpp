#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QWidget>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

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
// int main() {
//   std::vector<std::vector<std::string>> board = {
//       {"r", "n", "b", "q", "k", "b", "n", "r"},
//       {"p", "p", "p", "p", "p", "p", "p", "p"},
//       {"", "", "", "", "", "", "", ""},
//       {"", "", "", "", "", "", "", ""},
//       {"", "", "", "", "", "", "", ""},
//       {"", "", "", "", "", "", "", ""},
//       {"P", "P", "P", "P", "P", "P", "P", "P"},
//       {"R", "N", "B", "Q", "K", "B", "N", "R"}};
//
//   // bool output = CheckIfValidMove(1, 5, 3, 5 , board, 'b', 'p' );
//   bool output = CheckIfValidMove(1, 5, 3, 5, board, 'b', 'p');
//
//
//   std::cout << output << std::endl;
//   // QApplication app(argc, argv);
//   //
//   // QMainWindow mainWindow;
//   //
//   // // Get the current palette of the main window
//   // QPalette palette = mainWindow.palette();
//   //
//   // // Set the Window role color to black
//   // palette.setColor(QPalette::Window, Qt::white);
//   //
//   // // Apply the modified palette to the main window
//   // mainWindow.setPalette(palette);
//   //
//   // // Ensure the background is filled
//   // mainWindow.setAutoFillBackground(true);
//   //
//   // mainWindow.setWindowTitle("Black Window");
//   // mainWindow.show();
//
//   // return app.exec();
// }


int main() {
    std::vector<std::vector<std::string>> board = {
        {"r", "n", "b", "q", "k", "b", "n", "r"}, // row 0 black back rank
        {"p", "p", "p", "p", "p", "p", "p", "p"}, // row 1 black pawns
        {"", "", "", "", "", "", "", ""},         // row 2
        {"", "", "", "", "", "", "", ""},         // row 3
        {"", "", "", "", "", "", "", ""},         // row 4
        {"", "", "", "", "", "", "", ""},         // row 5
        {"P", "P", "P", "P", "P", "P", "P", "P"}, // row 6 white pawns
        {"R", "N", "B", "Q", "K", "B", "N", "R"}  // row 7 white back rank
    };

    // Test 1: black pawn from (1,5) to (3,5) — should be valid
    bool output = CheckIfValidMove(1, 5, 3, 5, board, 'b', 'p');
    std::cout << "Black pawn double step: " << output << "\n";

    // Test 2: white pawn from (6,0) to (4,0) — should be valid
    bool test2 = CheckIfValidMove(6, 0, 4, 0, board, 'w', 'P');
    std::cout << "White pawn double step: " << test2 << "\n";

    // Test 3: knight from (0,1) to (2,2) — should be valid
    bool test3 = CheckIfValidMove(0, 1, 2, 2, board, 'b', 'n');
    std::cout << "Black knight move: " << test3 << "\n";

    // Test 4: rook from (0,0) to (0,5) — should be invalid (pieces in way)
    bool test4 = CheckIfValidMove(0, 0, 0, 5, board, 'b', 'r');
    std::cout << "Black rook blocked move: " << test4 << "\n";

    // Test 5: rook from (0,0) to (0,0) — should be valid trivially? (same pos)
    bool test5 = CheckIfValidMove(0, 0, 0, 0, board, 'b', 'r');
    std::cout << "Rook no-move: " << test5 << "\n";

    return 0;
}

