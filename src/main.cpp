#include <QApplication>
#include <QGridLayout>
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>

int main(int argc, char *argv[]) {
  int height = 600;
  int weight = 600;
  QApplication app(argc, argv);

  QWidget *Centrlll = new QWidget;
  QGridLayout *Layout = new QGridLayout(Centrlll);

  QMainWindow window;
  window.resize(height, weight);

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      QPushButton *btn = new QPushButton();
      btn->setFixedSize(height / 8, weight / 8);
      bool light = (row + col) % 2 == 0;
      btn->setStyleSheet(light ? "background-color: #f0d9b5;"
                               : // Light square color
                             "background-color: #b58863;"); // Dark square
      Layout->addWidget(btn, row, col);
    }
  }
  // Layout->addWidget(btn, row, col);
  window.setCentralWidget(Centrlll);
  window.show();
  return app.exec();
}
