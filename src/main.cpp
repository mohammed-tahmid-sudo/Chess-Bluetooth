#include "qchar.h"
#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QIcon>
#include <QMouseEvent>
#include <QObject>
#include <QWidget>
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

  std::vector<std::vector<std::string>> board = {
      {"r.png", "n.png", "b.png", "q.png", "k.png", "b.png", "n.png", "r.png"},
      {"p.png", "p.png", "p.png", "p.png", "p.png", "p.png", "p.png", "p.png"},
      {"", "", "", "", "", "", "", ""},
      {"", "", "", "", "", "", "", ""},
      {"", "", "", "", "", "", "", ""},
      {"", "", "", "", "", "", "", ""},
      {"P.png", "P.png", "P.png", "P.png", "P.png", "P.png", "P.png", "P.png"},
      {"R.png", "N.png", "B.png", "Q.png", "K.png", "B.png", "N.png", "R.png"}};

  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {

      QPushButton *btn = new QPushButton();
      btn->setFixedSize(window.height() / 8, window.width() / 8);

      if (!board[x][y].empty()) {
        btn->setIcon(QIcon(":/resources/Asset Images/" + QString::fromStdString(board[x][y])));
	btn->setIconSize(btn->size());
      }

      if ((x + y) % 2 == 0)
        btn->setStyleSheet("background-color: white;");
      else
        btn->setStyleSheet("background-color: gray;");

      grid->addWidget(btn, x, y);
    }
  }
  window.setLayout(grid);
  window.show();
  return app.exec();
}
