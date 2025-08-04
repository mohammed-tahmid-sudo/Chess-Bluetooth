#include <QApplication>
#include <QFrame>
#include <QGridLayout>
#include <QMainWindow>
#include <QWidget>
#include <iostream>
#include <vector>

void print_tokens(const std::vector<std::vector<char>> &tokens) {
  std::cout << "[";
  bool first_line = true;
  for (const auto &line : tokens) {
    if (!first_line)
      std::cout << ", ";
    std::cout << "[";
    bool first_token = true;
    for (const auto &token : line) {
      if (!first_token)
        std::cout << ", ";
      std::cout << "'" << token << "'";
      first_token = false;
    }
    std::cout << "]";
    first_line = false;
  }
  std::cout << "]";
}
int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QWidget *central = new QWidget;
  QGridLayout *layout = new QGridLayout(central);

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      QFrame *square = new QLable;
      square->setFixedSize(50, 50);
      square->setStyleSheet((row + col) % 2 == 0 ? "background: white;"
                                                 : "background: gray;");
      square->setAlignment(Qt::AlignCenter);
      layout->addWidget(square, row, col);
    }
  }

  QMainWindow window;
  window.setWindowTitle("Default Window");
  window.setCentralWidget(central);
  window.resize(400, 300);

  window.show();
  return app.exec();
}
