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

char Race(std::string str) {
  for (char c : str) {
    if (!isupper(static_cast<unsigned char>(
            c))) { // Cast to unsigned char is important
      return 'l';  // Found a non-uppercase character
    }
  }
  return 'u';
}

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
    if (CurrentPosX == ExpectedPosX) {
      // doing for the Y value
      int y = (ExpectedPosY > CurrentPosY) ? 1 : -1;

      for (int i = CurrentPosY + y; i != ExpectedPosY; i += y) {
        // if (Board[CurrentPosX][y] ==) {
        // }
        if (!Board[CurrentPosX][i].empty()) {
          return false;
        }
      }
      return Board[ExpectedPosX][ExpectedPosY].empty() ||
             Board[ExpectedPosX][ExpectedPosY][0] == opposite;

    } else if (CurrentPosY == ExpectedPosY) {

      int x = (ExpectedPosX > CurrentPosX) ? 1 : -1;

      for (int i = CurrentPosX + x; i != ExpectedPosX; i += x) {
        // if (Board[CurrentPosX][y] ==) {
        // }
        if (!Board[i][CurrentPosY].empty()) {
          return false;
        }
      }

      return Board[ExpectedPosX][ExpectedPosY].empty() ||
             Board[ExpectedPosX][ExpectedPosY][0] == opposite;
    }
  } else if (name == 'n' || name == 'N') {
    int dx = std::abs(ExpectedPosX - CurrentPosX);
    int dy = std::abs(ExpectedPosY - CurrentPosY);
    if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2)) {
      return Board[ExpectedPosX][ExpectedPosY].empty() ||
             Board[ExpectedPosX][ExpectedPosY][0] == opposite;
    }
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
      {"P", "P", "P", "P", "P", "P", "P", ""},  // 6 white pawns
      {"R", "N", "B", "Q", "K", "B", "N", "R"}  // 7 white back rank
  };

  // Pawn tests
  std::cout << "Black pawn double step (1,5 -> 3,5): " 
            << CheckIfValidMove(1, 5, 3, 5, board, 'b', 'p') << "\n"; // 1
  std::cout << "White pawn double step (6,0 -> 4,0): " 
            << CheckIfValidMove(6, 0, 4, 0, board, 'w', 'P') << "\n"; // 1

  // Knight test (add knight logic first)
  std::cout << "Black knight move (0,1 -> 2,2): " 
            << CheckIfValidMove(0, 1, 2, 2, board, 'b', 'n') << "\n"; // 1

  // Rook tests
  // Clear path for rook horizontal move
  board[0][1] = "";
  board[0][2] = "";
  board[0][3] = "";
  board[0][4] = "";
  board[0][5] = "";

  std::cout << "Rook move clear path (0,0 -> 0,5): " 
            << CheckIfValidMove(0, 0, 0, 5, board, 'b', 'r') << "\n"; // 1

  // Blocked rook move (piece in path)
  board[0][3] = "p";
  std::cout << "Rook move blocked (0,0 -> 0,5): " 
            << CheckIfValidMove(0, 0, 0, 5, board, 'b', 'r') << "\n"; // 0

  // Rook move same square (invalid)
  std::cout << "Rook move same square (0,0 -> 0,0): " 
            << CheckIfValidMove(0, 0, 0, 0, board, 'b', 'r') << "\n"; // 0

  // TODO: add bishop, queen, king tests if logic added

  return 0;
}

