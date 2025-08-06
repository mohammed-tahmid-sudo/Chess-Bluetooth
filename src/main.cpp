#include <QApplication>
#include <iostream>
#include <QDebug>
#include <QGridLayout>
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <map>
#include <vector>

class ButtonEvenHandler : public QObject {
  // Q_OBJECT

public:
  explicit ButtonEvenHandler(QObject *parent = nullptr);

public slots:
  void IfButtonClicked() { std::cout  << "Button has been pressed\n"; }
};
ButtonEvenHandler::ButtonEvenHandler(QObject *parent) : QObject(parent) {}

int main(int argc, char *argv[]) {
  int height = 600;
  int weight = 600;
  QApplication app(argc, argv);

  QWidget *Centrlll = new QWidget;
  QGridLayout *Layout = new QGridLayout(Centrlll);

  QMainWindow window;
  window.resize(height, weight);

  std::vector<std::vector<std::string>> Board = {
      {"r", "n", "b", "q", "k", "b", "n", "r"}, // Black major pieces
      {"p", "p", "p", "p", "p", "p", "p", "p"}, // Black pawns
      {"0", "0", "0", "0", "0", "0", "0", "0"}, // Empty
      {"0", "0", "0", "0", "0", "0", "0", "0"},
      {"0", "0", "0", "0", "0", "0", "0", "0"},
      {"0", "0", "0", "0", "0", "0", "0", "0"},
      {"P", "P", "P", "P", "P", "P", "P", "P"}, // White pawns
      {"R", "N", "B", "Q", "K", "B", "N", "R"}  // White major pieces
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

      auto it = pieceMap.find(Board[x][y]);

      if (it != pieceMap.end()) {
        QPixmap pixmap("resources/Asset Images/" +
                       it->second);
        BTN->setIcon(QIcon(pixmap));
        BTN->setIconSize(pixmap.size());
      }
      ButtonEvenHandler *handler = new  ButtonEvenHandler;
      QObject::connect(BTN, &QPushButton::clicked, handler, &ButtonEvenHandler::IfButtonClicked);

      Layout->addWidget(BTN, x, y);
    }
  }
  // Layout->addWidget(btn, row, col);
  window.setCentralWidget(Centrlll);
  window.show();
  return app.exec();
}
