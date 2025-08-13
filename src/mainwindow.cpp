#include "mainwindow.h"
#include <QWidget>
#include <QApplication>
#include <QMessageBox>
#include <QSplitter>
#include <QGroupBox>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), moveCount(1), isMultiplayerMode(false)
{
    // Initialize Bluetooth components
    bluetoothManager = new BluetoothManager(this);
    bluetoothDialog = nullptr; // Created on demand
    
    setupUI();
    createMenus();
    setupStatusBar();
    
    // Connect signals
    connect(chessBoard, &ChessBoard::moveMade, this, &MainWindow::onMoveMade);
    connect(chessBoard, &ChessBoard::moveMadeWithCoords, this, &MainWindow::onMoveMadeWithCoords);
    connect(chessBoard, &ChessBoard::gameStatusChanged, this, &MainWindow::onGameStatusChanged);
    connect(newGameButton, &QPushButton::clicked, this, &MainWindow::newGame);
    
    // Connect Bluetooth signals
    connect(bluetoothManager, &BluetoothManager::messageReceived, this, &MainWindow::onBluetoothMessageReceived);
    connect(bluetoothManager, &BluetoothManager::remoteDeviceConnected, this, &MainWindow::onBluetoothConnected);
    connect(bluetoothManager, &BluetoothManager::remoteDeviceDisconnected, this, &MainWindow::onBluetoothDisconnected);
    
    // Set window properties
    setWindowTitle("Chess Game - Bluetooth Edition");
    setMinimumSize(900, 700);
    resize(1000, 750);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Create main layout
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    
    // Create splitter for resizable layout
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    mainLayout->addWidget(splitter);
    
    // Chess board widget
    chessBoard = new ChessBoard(this);
    splitter->addWidget(chessBoard);
    
    // Right panel
    QWidget *rightPanel = new QWidget(this);
    rightPanel->setMinimumWidth(250);
    rightPanel->setMaximumWidth(350);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    
    // Game controls
    QGroupBox *controlsGroup = new QGroupBox("Game Controls", this);
    QVBoxLayout *controlsLayout = new QVBoxLayout(controlsGroup);
    
    newGameButton = new QPushButton("New Game", this);
    newGameButton->setFont(QFont("Arial", 12, QFont::Bold));
    newGameButton->setStyleSheet("QPushButton { padding: 8px; background-color: #4CAF50; color: white; border: none; border-radius: 4px; }"
                                "QPushButton:hover { background-color: #45a049; }");
    
    undoButton = new QPushButton("Undo Move", this);
    undoButton->setFont(QFont("Arial", 10));
    undoButton->setStyleSheet("QPushButton { padding: 6px; background-color: #f44336; color: white; border: none; border-radius: 4px; }"
                             "QPushButton:hover { background-color: #da190b; }");
    undoButton->setEnabled(false); // TODO: Implement undo functionality
    
    controlsLayout->addWidget(newGameButton);
    controlsLayout->addWidget(undoButton);
    
    // Bluetooth button
    bluetoothButton = new QPushButton("Bluetooth Multiplayer", this);
    bluetoothButton->setFont(QFont("Arial", 10, QFont::Bold));
    bluetoothButton->setStyleSheet("QPushButton { padding: 8px; background-color: #2196F3; color: white; border: none; border-radius: 4px; }"
                                 "QPushButton:hover { background-color: #1976D2; }");
    connect(bluetoothButton, &QPushButton::clicked, this, &MainWindow::openBluetoothDialog);
    controlsLayout->addWidget(bluetoothButton);
    
    // Status label
    statusLabel = new QLabel("White to move", this);
    statusLabel->setFont(QFont("Arial", 11, QFont::Bold));
    statusLabel->setStyleSheet("QLabel { padding: 8px; background-color: #e3f2fd; border: 1px solid #2196f3; border-radius: 4px; }");
    statusLabel->setAlignment(Qt::AlignCenter);
    controlsLayout->addWidget(statusLabel);
    
    rightLayout->addWidget(controlsGroup);
    
    // Move history
    QGroupBox *historyGroup = new QGroupBox("Move History", this);
    QVBoxLayout *historyLayout = new QVBoxLayout(historyGroup);
    
    moveHistory = new QTextEdit(this);
    moveHistory->setReadOnly(true);
    moveHistory->setFont(QFont("Courier", 10));
    moveHistory->setStyleSheet("QTextEdit { background-color: #f5f5f5; border: 1px solid #ccc; }");
    moveHistory->setMaximumHeight(300);
    
    historyLayout->addWidget(moveHistory);
    rightLayout->addWidget(historyGroup);
    
    // AI Controls
    QGroupBox *aiGroup = new QGroupBox("AI Opponent", this);
    QVBoxLayout *aiLayout = new QVBoxLayout(aiGroup);
    
    // Enable AI checkbox
    aiEnabledCheckBox = new QCheckBox("Enable AI Opponent", this);
    aiEnabledCheckBox->setFont(QFont("Arial", 10, QFont::Bold));
    connect(aiEnabledCheckBox, &QCheckBox::toggled, this, &MainWindow::onAIEnabledChanged);
    aiLayout->addWidget(aiEnabledCheckBox);
    
    // AI Color selection
    QLabel *colorLabel = new QLabel("AI plays as:", this);
    aiLayout->addWidget(colorLabel);
    
    aiColorGroup = new QButtonGroup(this);
    aiWhiteRadio = new QRadioButton("White", this);
    aiBlackRadio = new QRadioButton("Black", this);
    aiBlackRadio->setChecked(true); // Default: AI plays black
    
    aiColorGroup->addButton(aiWhiteRadio, 0);
    aiColorGroup->addButton(aiBlackRadio, 1);
    connect(aiWhiteRadio, &QRadioButton::toggled, this, &MainWindow::onAIColorChanged);
    connect(aiBlackRadio, &QRadioButton::toggled, this, &MainWindow::onAIColorChanged);
    
    QHBoxLayout *colorLayout = new QHBoxLayout();
    colorLayout->addWidget(aiWhiteRadio);
    colorLayout->addWidget(aiBlackRadio);
    aiLayout->addLayout(colorLayout);
    
    // AI Difficulty selection
    QLabel *difficultyLabel = new QLabel("Difficulty:", this);
    aiLayout->addWidget(difficultyLabel);
    
    aiDifficultyComboBox = new QComboBox(this);
    aiDifficultyComboBox->addItem("Easy");
    aiDifficultyComboBox->addItem("Medium");
    aiDifficultyComboBox->addItem("Hard");
    aiDifficultyComboBox->addItem("Expert");
    aiDifficultyComboBox->setCurrentText("Medium");
    connect(aiDifficultyComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onAIDifficultyChanged);
    aiLayout->addWidget(aiDifficultyComboBox);
    
    // Initially disable AI controls
    colorLabel->setEnabled(false);
    aiWhiteRadio->setEnabled(false);
    aiBlackRadio->setEnabled(false);
    difficultyLabel->setEnabled(false);
    aiDifficultyComboBox->setEnabled(false);
    
    rightLayout->addWidget(aiGroup);
    
    // Add stretch to push everything to the top
    rightLayout->addStretch();
    
    splitter->addWidget(rightPanel);
    
    // Set initial sizes
    splitter->setSizes({600, 300});
}

void MainWindow::createMenus()
{
    // File menu
    QMenu *fileMenu = menuBar()->addMenu("&File");
    
    QAction *newGameAction = new QAction("&New Game", this);
    newGameAction->setShortcut(QKeySequence::New);
    newGameAction->setStatusTip("Start a new chess game");
    connect(newGameAction, &QAction::triggered, this, &MainWindow::newGame);
    fileMenu->addAction(newGameAction);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip("Exit the application");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);
    
    // Multiplayer menu
    QMenu *multiplayerMenu = menuBar()->addMenu("&Multiplayer");
    
    QAction *bluetoothAction = new QAction("&Bluetooth Connection", this);
    bluetoothAction->setStatusTip("Connect to another player via Bluetooth");
    connect(bluetoothAction, &QAction::triggered, this, &MainWindow::openBluetoothDialog);
    multiplayerMenu->addAction(bluetoothAction);
    
    // Help menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    
    QAction *aboutAction = new QAction("&About", this);
    aboutAction->setStatusTip("Show information about this application");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    helpMenu->addAction(aboutAction);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("Ready to play chess!", 2000);
}

void MainWindow::newGame()
{
    int reply = QMessageBox::question(this, "New Game", 
                                     "Are you sure you want to start a new game?",
                                     QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        chessBoard->resetBoard();
        moveHistory->clear();
        moveCount = 1;
        statusBar()->showMessage("New game started", 2000);
    }
}

void MainWindow::onMoveMade(QString moveNotation)
{
    // Add move to history
    QString moveText;
    if (moveCount % 2 == 1) { // White's move
        moveText = QString("%1. %2").arg((moveCount + 1) / 2).arg(moveNotation);
    } else { // Black's move
        moveText = QString("    %1").arg(moveNotation);
    }
    
    moveHistory->append(moveText);
    
    // Scroll to bottom
    QTextCursor cursor = moveHistory->textCursor();
    cursor.movePosition(QTextCursor::End);
    moveHistory->setTextCursor(cursor);
    
    moveCount++;
    
    statusBar()->showMessage(QString("Move made: %1").arg(moveNotation), 3000);
    
}

void MainWindow::onMoveMadeWithCoords(QString moveNotation, int fromRow, int fromCol, int toRow, int toCol)
{
    // Send move over Bluetooth if connected
    if (isMultiplayerMode && bluetoothManager && bluetoothManager->isConnected()) {
        bluetoothManager->sendMove(fromRow, fromCol, toRow, toCol);
        statusBar()->showMessage("Move sent to remote player", 2000);
    }
}

void MainWindow::onGameStatusChanged(QString status)
{
    statusLabel->setText(status);
    statusBar()->showMessage(status, 3000);
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About Chess Game",
                       "<h2>Chess Game - Bluetooth Edition</h2>"
                       "<p>A modern chess game built with Qt and C++.</p>"
                       "<p>Features:</p>"
                       "<ul>"
                       "<li>Complete chess rule implementation</li>"
                       "<li>Beautiful graphical interface</li>"
                       "<li>Move history tracking</li>"
                       "<li>Piece selection highlighting</li>"
                       "</ul>"
                       "<p>Built with Qt and love ❤️</p>");
}

// AI Control Implementations
void MainWindow::onAIEnabledChanged(bool enabled)
{
    chessBoard->setAIEnabled(enabled);
    
    // Enable/disable AI controls
    aiWhiteRadio->setEnabled(enabled);
    aiBlackRadio->setEnabled(enabled);
    aiDifficultyComboBox->setEnabled(enabled);
    
    // Find and enable/disable labels
    QList<QLabel*> labels = findChildren<QLabel*>();
    for (QLabel* label : labels) {
        if (label->text() == "AI plays as:" || label->text() == "Difficulty:") {
            label->setEnabled(enabled);
        }
    }
    
    if (enabled) {
        statusBar()->showMessage("AI Opponent enabled", 3000);
        // Set initial AI configuration
        onAIColorChanged();
        onAIDifficultyChanged(aiDifficultyComboBox->currentText());
    } else {
        statusBar()->showMessage("AI Opponent disabled", 3000);
    }
}

void MainWindow::onAIDifficultyChanged(const QString& difficulty)
{
    ChessAI::Difficulty aiDifficulty;
    
    if (difficulty == "Easy") {
        aiDifficulty = ChessAI::EASY;
    } else if (difficulty == "Medium") {
        aiDifficulty = ChessAI::MEDIUM;
    } else if (difficulty == "Hard") {
        aiDifficulty = ChessAI::HARD;
    } else { // Expert
        aiDifficulty = ChessAI::EXPERT;
    }
    
    chessBoard->setAIDifficulty(aiDifficulty);
    statusBar()->showMessage(QString("AI difficulty set to %1").arg(difficulty), 2000);
}

void MainWindow::onAIColorChanged()
{
    char aiColor = aiWhiteRadio->isChecked() ? 'w' : 'b';
    chessBoard->setAIColor(aiColor);
    
    QString colorName = (aiColor == 'w') ? "White" : "Black";
    statusBar()->showMessage(QString("AI will play as %1").arg(colorName), 2000);
}

// Bluetooth Implementation
void MainWindow::openBluetoothDialog()
{
    if (!bluetoothDialog) {
        bluetoothDialog = new BluetoothDialog(bluetoothManager, this);
    }
    
    bluetoothDialog->show();
    bluetoothDialog->raise();
    bluetoothDialog->activateWindow();
}

void MainWindow::onBluetoothMessageReceived(const GameMessage& message)
{
    switch (message.type) {
        case GameMessage::MOVE:
            // Apply the remote move to the board
            if (message.fromRow >= 0 && message.fromCol >= 0 && 
                message.toRow >= 0 && message.toCol >= 0) {
                if (chessBoard->makeExternalMove(message.fromRow, message.fromCol, 
                                                message.toRow, message.toCol)) {
                    statusBar()->showMessage("Move received and applied from remote player", 2000);
                } else {
                    statusBar()->showMessage("Invalid move received from remote player", 3000);
                }
            }
            break;
            
        case GameMessage::NEW_GAME:
            {
                int reply = QMessageBox::question(this, "New Game Request", 
                                                 "Remote player wants to start a new game. Accept?",
                                                 QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes) {
                    chessBoard->resetBoard();
                    moveHistory->clear();
                    moveCount = 1;
                    statusBar()->showMessage("New game started by remote player", 3000);
                }
            }
            break;
            
        case GameMessage::DISCONNECT:
            statusBar()->showMessage("Remote player disconnected", 3000);
            isMultiplayerMode = false;
            break;
            
        case GameMessage::CHAT:
            // Chat messages are handled by the Bluetooth dialog
            break;
            
        default:
            break;
    }
}

void MainWindow::onBluetoothConnected(const QString& deviceName)
{
    isMultiplayerMode = true;
    
    // Disable AI when connected to another player
    if (chessBoard->isAIEnabled()) {
        aiEnabledCheckBox->setChecked(false);
        chessBoard->setAIEnabled(false);
    }
    
    statusBar()->showMessage(QString("Connected to %1 for multiplayer game").arg(deviceName), 5000);
    
    // Update window title to show multiplayer mode
    setWindowTitle(QString("Chess Game - Connected to %1").arg(deviceName));
}

void MainWindow::onBluetoothDisconnected()
{
    isMultiplayerMode = false;
    statusBar()->showMessage("Disconnected from multiplayer game", 3000);
    
    // Restore original window title
    setWindowTitle("Chess Game - Bluetooth Edition");
}
