#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QWidget>
#include <cmath>
#include <iostream>
#include <iterator>
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
          Board[ExpectedPosX][ExpectedPosY][0] == opposite)
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
          Board[ExpectedPosX][ExpectedPosY][0] == opposite)
        return true;
    }
  } else if (name == 'r' || name == 'R') {
    if (Race == 'w') {
      auto distance = std::distance(Board.begin() + CurrentPosX,
                                    Board.begin() + ExpectedPosX);

      if (CurrentPosY == ExpectedPosY) {
        if (distance) {
          for (int i = 0; i < distance; i++) {
            if (Board[i].empty()) {
              return true;
            }
          }
        }
      }
    }
  } else if (Race == 'b') {
    // I'll be implemention that later too
  }

  return 0;
}
int main() {

  std::vector<std::vector<std::string>> board = {
      {"r", "n", "b", "q", "k", "b", "n", "r"}, // 0 black back rank
      {"p", "p", "p", "p", "p", "p", "p", "p"}, // 1 black pawns
      {"", "", "", "", "", "", "", ""},         // 2
      {"", "", "", "", "", "", "", ""},         // 3
      {"", "", "", "", "", "", "", ""},         // 4
      {"", "", "", "", "", "", "", ""},         // 5
      {"P", "P", "P", "P", "P", "P", "P", "P"}, // 6 white pawns
      {"R", "N", "B", "Q", "K", "B", "N", "R"}  // 7 white back rank
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
