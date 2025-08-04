#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QMainWindow window;
  window.resize(300, 300);
  window.show();
  return app.exec();
}
