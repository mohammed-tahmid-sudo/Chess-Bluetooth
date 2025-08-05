#include <QApplication>
#include <QGridLayout>
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
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
      {"r", "n", "b", "q", "k", "b", "n", "r"}, // Black major pieces
      {"p", "p", "p", "p", "p", "p", "p", "p"}, // Black pawns
      {"0", "0", "0", "0", "0", "0", "0", "0"}, // Empty
      {"0", "0", "0", "0", "0", "0", "0", "0"},
      {"0", "0", "0", "0", "0", "0", "0", "0"},
      {"0", "0", "0", "0", "0", "0", "0", "0"},
      {"P", "P", "P", "P", "P", "P", "P", "P"}, // White pawns
      {"R", "N", "B", "Q", "K", "B", "N", "R"}  // White major pieces
  };

  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      QPushButton *BTN = new QPushButton();
      BTN->setFixedSize(height / 8, weight / 8);

      if ((x + y) % 2 == 0)
        BTN->setStyleSheet("background-color: #ffffff");
      else
        BTN->setStyleSheet("background-color: #779556");

      if (Board[x][y] == "p") {
        QPixmap Pixmap("/home/tahmid/programming_files/Chess-bluetooth/"
                       "resources/Asset Images/p.png");
        QIcon icon(Pixmap);
        BTN->setIcon(icon);
        BTN->setIconSize(Pixmap.size());
      } else if (Board[x][y] == "P") {
        QPixmap Pixmap("/home/tahmid/programming_files/Chess-bluetooth/"
                       "resources/Asset Images/P.png");
        QIcon icon(Pixmap);
        BTN->setIcon(icon);
        BTN->setIconSize(Pixmap.size());
      } else if (Board[x][y] == "k") {
        QPixmap Pixmap("/home/tahmid/programming_files/Chess-bluetooth/"
                       "resources/Asset Images/k.png");
        QIcon icon(Pixmap);
        BTN->setIcon(icon);
        BTN->setIconSize(Pixmap.size());
      } else if (Board[x][y] == "K") {
        QPixmap Pixmap("/home/tahmid/programming_files/Chess-bluetooth/"
                       "resources/Asset Images/K.png");
        QIcon icon(Pixmap);
        BTN->setIcon(icon);
        BTN->setIconSize(Pixmap.size());
      } else if (Board[x][y] == "r") {
        QPixmap Pixmap("/home/tahmid/programming_files/Chess-bluetooth/resources/Asset Images/r.png");
        QIcon icon(Pixmap);
        BTN->setIcon(icon);
        BTN->setIconSize(Pixmap.size());
      } else if (Board[x][y] == "R") {
        QPixmap Pixmap("/home/tahmid/programming_files/Chess-bluetooth/resources/Asset Images/R.png");
        QIcon icon(Pixmap);
        BTN->setIcon(icon);
        BTN->setIconSize(Pixmap.size());
      } else if (Board[x][y] == "b") {
        QPixmap Pixmap("/home/tahmid/programming_files/Chess-bluetooth/resources/Asset Images/b.png");
        QIcon icon(Pixmap);
        BTN->setIcon(icon);
        BTN->setIconSize(Pixmap.size());
      } else if (Board[x][y] == "B") {
        QPixmap Pixmap("/home/tahmid/programming_files/Chess-bluetooth/resources/Asset Images/B.png");
        QIcon icon(Pixmap);
        BTN->setIcon(icon);
        BTN->setIconSize(Pixmap.size());
      } else if (Board[x][y] == "n") {
        QPixmap Pixmap("/home/tahmid/programming_files/Chess-bluetooth/resources/Asset Images/n.png");
        QIcon icon(Pixmap);
        BTN->setIcon(icon);
        BTN->setIconSize(Pixmap.size());
      } else if (Board[x][y] == "N") {
        QPixmap Pixmap("/home/tahmid/programming_files/Chess-bluetooth/resources/Asset Images/N.png");
        QIcon icon(Pixmap);
        BTN->setIcon(icon);
        BTN->setIconSize(Pixmap.size());
      } else if (Board[x][y] == "q") {
        QPixmap Pixmap("/home/tahmid/programming_files/Chess-bluetooth/resources/Asset Images/q.png");
        QIcon icon(Pixmap);
        BTN->setIcon(icon);
        BTN->setIconSize(Pixmap.size());
      }else if (Board[x][y] == "Q") {
        QPixmap Pixmap("/home/tahmid/programming_files/Chess-bluetooth/resources/Asset Images/Q.png");
        QIcon icon(Pixmap);
        BTN->setIcon(icon);
        BTN->setIconSize(Pixmap.size());
      } else if (Board[x][y] == "b") {
        QPixmap Pixmap("/home/tahmid/programming_files/Chess-bluetooth/resources/Asset Images/b.png");
        QIcon icon(Pixmap);
        BTN->setIcon(icon);
        BTN->setIconSize(Pixmap.size());
      } else if (Board[x][y] == "b") {
        QPixmap Pixmap("/home/tahmid/programming_files/Chess-bluetooth/resources/Asset Images/B.png");
        QIcon icon(Pixmap);
        BTN->setIcon(icon);
        BTN->setIconSize(Pixmap.size());
      } 

      Layout->addWidget(BTN, x, y); } }
  // Layout->addWidget(btn, row, col);
  window.setCentralWidget(Centrlll);
  window.show();
  return app.exec();
}
