#include <QApplication>
#include <QPalette>
#include <QMainWindow>
#include <QWidget>
#include <iostream>
#include <map>
#include <string>
#include <vector>

void CheckIfValidMove(int CurrentPosX, int CurrentPoxY, int ExpectingPosX, int ExpectedPosY, std::vector<std::vector<std::string>>, char Race, ) { 


}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QMainWindow mainWindow;

  // Get the current palette of the main window
  QPalette palette = mainWindow.palette();

  // Set the Window role color to black
  palette.setColor(QPalette::Window, Qt::black);

  // Apply the modified palette to the main window
  mainWindow.setPalette(palette);

  // Ensure the background is filled
  mainWindow.setAutoFillBackground(true);

  mainWindow.setWindowTitle("Black Window");
  mainWindow.show();

  return app.exec();
}
