#include <QApplication>
#include <QDebug>
#include <QGridLayout>
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <iostream>
#include <map>
#include <vector>


int main(int argc, char *argv[]) {
  int height = 600;
  int weight = 600;
  QApplication app(argc, argv);

  QWidget *Centrlll = new QWidget;
  QGridLayout *Layout = new QGridLayout(Centrlll);

  QMainWindow window;
  window.resize(height, weight);

  std::vector<std::vector<std::string>> Board = {
      {"r.png", "n.png", "b.png", "q.png", "k.png", "b.png", "n.png",
       "r.png"}, // Black major pieces
      {"p.png", "p.png", "p.png", "p.png", "p.png", "p.png", "p.png",
       "p.png"},                                // Black pawns
      {"0", "0", "0", "0", "0", "0", "0", "0"}, // Empty
      {"0", "0", "0", "0", "0", "0", "0", "0"},
      {"0", "0", "0", "0", "0", "0", "0", "0"},
      {"0", "0", "0", "0", "0", "0", "0", "0"},
      {"P.png", "P.png", "P.png", "P.png", "P.png", "P.png", "P.png",
       "P.png"}, // White pawns
      {"R.png", "N.png", "B.png", "Q.png", "K.png", "B.png", "N.png", "R.png"}
      // White major pieces
  };
  std::map<std::string, QString> pieceMap = {
      {"p", "p.png"}, {"P", "P.png"}, {"k", "k.png"}, {"K", "K.png"},
      {"r", "r.png"}, {"R", "R.png"}, {"n", "n.png"}, {"N", "N.png"},
      {"b", "b.png"}, {"B", "B.png"}, {"q", "q.png"}, {"Q", "Q.png"}};

  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      QPushButton *BTN = new QPushButton();
      BTN->setFixedSize(height / 8, weight / 8);

      if ((x + y) % 2 == 0)
        BTN->setStyleSheet("background-color: #ffffff");
      else
        BTN->setStyleSheet("background-color: #779556");

      if (!Board[x][y].empty()) {

        std::cout << "resources/Asset Images/" + Board[x][y] << std::endl;
        // QPixmap pixmap("resources/Asset Images/" + Board[x][y]);
	QPixmap pixmap(QString("resources/Asset Images/") + QString::fromStdString(Board[x][y]));

        BTN->setIcon(QIcon(pixmap));
        BTN->setIconSize(pixmap.size());
      }

      Layout->addWidget(BTN, x, y);
    }
  }
  // Layout->addWidget(btn, row, col);
  window.setCentralWidget(Centrlll);
  window.show();
  return app.exec();
}
