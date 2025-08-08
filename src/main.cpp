#include <QApplication>
#include <QGridLayout>
#include <QIcon>
#include <QMouseEvent>
#include <QObject>
#include <QPushButton>
#include <QWidget>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  int height = 500;
  int weight = 600;

  // ChessBluetoothApplication window;
  QWidget window;
  window.resize(height, weight);
  // Create a main window/widget
  QGridLayout *grid = new QGridLayout(&window);

  // std::vector<std::vector<std::string>> board = {
  //     {"r.png", "n.png", "b.png", "q.png", "k.png", "b.png", "n.png", "r.png"},
  //     {"p.png", "p.png", "p.png", "p.png", "p.png", "p.png", "p.png", "p.png"},
  //     {"", "", "", "", "", "", "", ""},
  //     {"", "", "", "", "", "", "", ""},
  //     {"", "", "", "", "", "", "", ""},
  //     {"", "", "", "", "", "", "", ""},
  //     {"P.png", "P.png", "P.png", "P.png", "P.png", "P.png", "P.png", "P.png"},
  //     {"R.png", "N.png", "B.png", "Q.png", "K.png", "B.png", "N.png", "R.png"}};
  

  std::vector<std::vector<std::string>> board = {
      {"RB.png", "NB.png", "BN.png", "QB.png", "KB.png", "BB.png", "NB.png", "RB.png"},
      {"PB.png", "PB.png", "PB.png", "PB.png", "PB.png", "PB.png", "PB.png", "PB.png"},
      {"", "", "", "", "", "", "", ""},
      {"", "", "", "", "", "", "", ""},
      {"", "", "", "", "", "", "", ""},
      {"", "", "", "", "", "", "", ""},
      {"PW.png", "PW.png", "PW.png", "PW.png", "PW.png", "PW.png", "PW.png", "PW.png"},
      {"RW.png", "NW.png", "BW.png", "QW.png", "KW.png", "BW.png", "NW.png", "RW.png"}};
  
  std::vector<std::vector<QPushButton *>> buttons(
      8, std::vector<QPushButton *>(8));

  bool IsAnyButtonsPressed = false;
  int PreviousButtonLocationX = 0;
  int PreviousButtonLocationY = 0;
  // std::string holder;

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
        std::cout << "Button is pressed" << std::endl;
        if (IsAnyButtonsPressed) {
          if (board[x][y].empty()) {
            // holder = board[x][y]
            board[x][y] =
                board[PreviousButtonLocationX][PreviousButtonLocationY];
            board[PreviousButtonLocationX][PreviousButtonLocationY] = "";

            // Update icons after move:
            buttons[x][y]->setIcon(QIcon(":/resources/Asset Images/" +
                                         QString::fromStdString(board[x][y])));
            buttons[x][y]->setIconSize(buttons[x][y]->size());
            buttons[PreviousButtonLocationX][PreviousButtonLocationY]->setIcon(
                QIcon());
            IsAnyButtonsPressed = false;
          }
          IsAnyButtonsPressed = false;
        } else {
          IsAnyButtonsPressed = true;
          PreviousButtonLocationX = x;
          PreviousButtonLocationY = y;
        }
      });
    }
  }

  window.setLayout(grid);
  window.show();
  return app.exec();
}
