#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QMenuBar>
#include <QAction>
#include <QStatusBar>
#include <QCheckBox>
#include <QComboBox>
#include <QButtonGroup>
#include <QRadioButton>
#include "chessboard.h"
#include "bluetoothmanager.h"
#include "bluetoothdialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newGame();
    void onMoveMade(QString moveNotation);
    void onMoveMadeWithCoords(QString moveNotation, int fromRow, int fromCol, int toRow, int toCol);
    void onGameStatusChanged(QString status);
    void showAbout();
    
    // AI control slots
    void onAIEnabledChanged(bool enabled);
    void onAIDifficultyChanged(const QString& difficulty);
    void onAIColorChanged();
    
    // Bluetooth slots
    void openBluetoothDialog();
    void onBluetoothMessageReceived(const GameMessage& message);
    void onBluetoothConnected(const QString& deviceName);
    void onBluetoothDisconnected();

private:
    void setupUI();
    void createMenus();
    void setupStatusBar();
    
    ChessBoard *chessBoard;
    QTextEdit *moveHistory;
    QLabel *statusLabel;
    QPushButton *newGameButton;
    QPushButton *undoButton;
    
    // AI controls
    QCheckBox *aiEnabledCheckBox;
    QComboBox *aiDifficultyComboBox;
    QRadioButton *aiWhiteRadio;
    QRadioButton *aiBlackRadio;
    QButtonGroup *aiColorGroup;
    
    // Bluetooth components
    BluetoothManager *bluetoothManager;
    BluetoothDialog *bluetoothDialog;
    QPushButton *bluetoothButton;
    bool isMultiplayerMode;
    
    int moveCount;
};

#endif // MAINWINDOW_H
