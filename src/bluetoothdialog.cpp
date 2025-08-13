#include "bluetoothdialog.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QApplication>
#include <QTime>
#include <QScrollBar>

BluetoothDialog::BluetoothDialog(BluetoothManager* btManager, QWidget *parent)
    : QDialog(parent)
    , bluetoothManager(btManager)
    , statusUpdateTimer(nullptr)
{
    setWindowTitle("Bluetooth Multiplayer Chess");
    setMinimumSize(500, 400);
    resize(600, 500);
    
    setupUI();
    
    // Connect Bluetooth manager signals
    connect(bluetoothManager, &BluetoothManager::connectionStateChanged,
            this, &BluetoothDialog::onConnectionStateChanged);
    connect(bluetoothManager, &BluetoothManager::deviceDiscovered,
            this, &BluetoothDialog::onDeviceDiscovered);
    connect(bluetoothManager, &BluetoothManager::discoveryFinished,
            this, &BluetoothDialog::onDiscoveryFinished);
    connect(bluetoothManager, &BluetoothManager::messageReceived,
            this, &BluetoothDialog::onMessageReceived);
    connect(bluetoothManager, &BluetoothManager::errorOccurred,
            this, &BluetoothDialog::onErrorOccurred);
    connect(bluetoothManager, &BluetoothManager::remoteDeviceConnected,
            this, &BluetoothDialog::onRemoteDeviceConnected);
    connect(bluetoothManager, &BluetoothManager::remoteDeviceDisconnected,
            this, &BluetoothDialog::onRemoteDeviceDisconnected);
    connect(bluetoothManager, &BluetoothManager::bluetoothStateChanged,
            this, &BluetoothDialog::onBluetoothStateChanged);
    
    // Setup status update timer
    statusUpdateTimer = new QTimer(this);
    connect(statusUpdateTimer, &QTimer::timeout, this, &BluetoothDialog::onRefreshStatus);
    statusUpdateTimer->start(1000); // Update every second
    
    // Initial status update
    onRefreshStatus();
}

BluetoothDialog::~BluetoothDialog()
{
}

void BluetoothDialog::closeEvent(QCloseEvent* event)
{
    // Don't close if connected - just hide
    if (bluetoothManager->isConnected()) {
        hide();
        event->ignore();
    } else {
        event->accept();
    }
}

void BluetoothDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Connection status at top
    QGroupBox* statusGroup = new QGroupBox("Connection Status", this);
    QGridLayout* statusLayout = new QGridLayout(statusGroup);
    
    connectionStatusLabel = new QLabel("Disconnected", this);
    connectionStatusLabel->setStyleSheet("QLabel { font-weight: bold; color: red; }");
    statusLayout->addWidget(new QLabel("Status:"), 0, 0);
    statusLayout->addWidget(connectionStatusLabel, 0, 1);
    
    remoteDeviceLabel = new QLabel("None", this);
    statusLayout->addWidget(new QLabel("Remote Device:"), 1, 0);
    statusLayout->addWidget(remoteDeviceLabel, 1, 1);
    
    mainLayout->addWidget(statusGroup);
    
    // Tab widget for different functions
    tabWidget = new QTabWidget(this);
    
    setupServerTab();
    setupClientTab();
    setupChatTab();
    
    tabWidget->addTab(serverTab, "Host Game");
    tabWidget->addTab(clientTab, "Join Game");
    tabWidget->addTab(chatTab, "Chat");
    
    mainLayout->addWidget(tabWidget);
    
    // Close button
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton* closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &BluetoothDialog::close);
    buttonLayout->addWidget(closeButton);
    
    mainLayout->addLayout(buttonLayout);
}

void BluetoothDialog::setupServerTab()
{
    serverTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(serverTab);
    
    // Local device info
    QGroupBox* deviceGroup = new QGroupBox("Local Device", serverTab);
    QVBoxLayout* deviceLayout = new QVBoxLayout(deviceGroup);
    
    localDeviceLabel = new QLabel("Loading...", serverTab);
    deviceLayout->addWidget(localDeviceLabel);
    
    layout->addWidget(deviceGroup);
    
    // Server controls
    QGroupBox* serverGroup = new QGroupBox("Host Game", serverTab);
    QVBoxLayout* serverLayout = new QVBoxLayout(serverGroup);
    
    QLabel* infoLabel = new QLabel("Start a server to allow other players to connect to your game.", serverTab);
    infoLabel->setWordWrap(true);
    serverLayout->addWidget(infoLabel);
    
    startServerButton = new QPushButton("Start Server", serverTab);
    startServerButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; font-weight: bold; }");
    connect(startServerButton, &QPushButton::clicked, this, &BluetoothDialog::onStartServerClicked);
    serverLayout->addWidget(startServerButton);
    
    stopServerButton = new QPushButton("Stop Server", serverTab);
    stopServerButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; }");
    stopServerButton->setEnabled(false);
    connect(stopServerButton, &QPushButton::clicked, this, &BluetoothDialog::onStopServerClicked);
    serverLayout->addWidget(stopServerButton);
    
    serverStatusLabel = new QLabel("Server stopped", serverTab);
    serverStatusLabel->setStyleSheet("QLabel { color: #666; font-style: italic; }");
    serverLayout->addWidget(serverStatusLabel);
    
    layout->addWidget(serverGroup);
    layout->addStretch();
}

void BluetoothDialog::setupClientTab()
{
    clientTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(clientTab);
    
    // Search controls
    QGroupBox* searchGroup = new QGroupBox("Find Games", clientTab);
    QVBoxLayout* searchLayout = new QVBoxLayout(searchGroup);
    
    QLabel* searchInfo = new QLabel("Search for nearby devices hosting chess games.", clientTab);
    searchInfo->setWordWrap(true);
    searchLayout->addWidget(searchInfo);
    
    QHBoxLayout* searchButtonLayout = new QHBoxLayout();
    searchButton = new QPushButton("Search for Devices", clientTab);
    searchButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 8px; font-weight: bold; }");
    connect(searchButton, &QPushButton::clicked, this, &BluetoothDialog::onSearchDevicesClicked);
    searchButtonLayout->addWidget(searchButton);
    
    searchProgressBar = new QProgressBar(clientTab);
    searchProgressBar->setRange(0, 0); // Indeterminate progress
    searchProgressBar->setVisible(false);
    searchButtonLayout->addWidget(searchProgressBar);
    
    searchLayout->addLayout(searchButtonLayout);
    layout->addWidget(searchGroup);
    
    // Device list
    QGroupBox* devicesGroup = new QGroupBox("Available Devices", clientTab);
    QVBoxLayout* devicesLayout = new QVBoxLayout(devicesGroup);
    
    deviceListWidget = new QListWidget(clientTab);
    connect(deviceListWidget, &QListWidget::itemClicked, this, &BluetoothDialog::onDeviceListItemClicked);
    devicesLayout->addWidget(deviceListWidget);
    
    // Connection controls
    QHBoxLayout* connectionLayout = new QHBoxLayout();
    connectButton = new QPushButton("Connect", clientTab);
    connectButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; }");
    connectButton->setEnabled(false);
    connect(connectButton, &QPushButton::clicked, this, &BluetoothDialog::onConnectToDeviceClicked);
    connectionLayout->addWidget(connectButton);
    
    disconnectButton = new QPushButton("Disconnect", clientTab);
    disconnectButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; }");
    disconnectButton->setEnabled(false);
    connect(disconnectButton, &QPushButton::clicked, this, &BluetoothDialog::onDisconnectClicked);
    connectionLayout->addWidget(disconnectButton);
    
    devicesLayout->addLayout(connectionLayout);
    
    clientStatusLabel = new QLabel("Ready to search", clientTab);
    clientStatusLabel->setStyleSheet("QLabel { color: #666; font-style: italic; }");
    devicesLayout->addWidget(clientStatusLabel);
    
    layout->addWidget(devicesGroup);
}

void BluetoothDialog::setupChatTab()
{
    chatTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(chatTab);
    
    // Chat display
    chatDisplay = new QTextEdit(chatTab);
    chatDisplay->setReadOnly(true);
    chatDisplay->setStyleSheet("QTextEdit { background-color: #f9f9f9; border: 1px solid #ddd; }");
    layout->addWidget(chatDisplay);
    
    // Chat input
    QHBoxLayout* inputLayout = new QHBoxLayout();
    
    chatInput = new QLineEdit(chatTab);
    chatInput->setPlaceholderText("Type a message...");
    chatInput->setEnabled(false);
    connect(chatInput, &QLineEdit::returnPressed, this, &BluetoothDialog::onSendChatMessage);
    inputLayout->addWidget(chatInput);
    
    sendChatButton = new QPushButton("Send", chatTab);
    sendChatButton->setEnabled(false);
    sendChatButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 6px 12px; }");
    connect(sendChatButton, &QPushButton::clicked, this, &BluetoothDialog::onSendChatMessage);
    inputLayout->addWidget(sendChatButton);
    
    layout->addLayout(inputLayout);
    
    // Initial message
    addChatMessage("System", "Connect to another player to start chatting.");
}

// UI Event Handlers

void BluetoothDialog::onStartServerClicked()
{
    bluetoothManager->startServer();
}

void BluetoothDialog::onStopServerClicked()
{
    bluetoothManager->stopServer();
}

void BluetoothDialog::onSearchDevicesClicked()
{
    deviceListWidget->clear();
    availableDevices.clear();
    bluetoothManager->startDiscovery();
}

void BluetoothDialog::onConnectToDeviceClicked()
{
    if (selectedDevice.isValid()) {
        bluetoothManager->connectToDevice(selectedDevice);
    }
}

void BluetoothDialog::onDisconnectClicked()
{
    bluetoothManager->disconnect();
}

void BluetoothDialog::onDeviceListItemClicked(QListWidgetItem* item)
{
    if (!item) return;
    
    int index = deviceListWidget->row(item);
    if (index >= 0 && index < availableDevices.size()) {
        selectedDevice = availableDevices[index];
        connectButton->setEnabled(true);
    }
}

void BluetoothDialog::onSendChatMessage()
{
    QString message = chatInput->text().trimmed();
    if (message.isEmpty() || !bluetoothManager->isConnected()) return;
    
    bluetoothManager->sendChatMessage(message);
    addChatMessage("You", message);
    chatInput->clear();
}

void BluetoothDialog::onRefreshStatus()
{
    updateConnectionStatus();
    
    // Update local device info
    if (bluetoothManager->isBluetoothAvailable()) {
        QString deviceName = bluetoothManager->getLocalDeviceName();
        if (deviceName.isEmpty()) {
            deviceName = "Unknown Device";
        }
        localDeviceLabel->setText(QString("Name: %1").arg(deviceName));
    } else {
        localDeviceLabel->setText("Bluetooth not available");
    }
}

// Bluetooth Manager Event Handlers

void BluetoothDialog::onConnectionStateChanged(BluetoothManager::ConnectionState state)
{
    updateConnectionStatus();
    
    switch (state) {
        case BluetoothManager::Disconnected:
            startServerButton->setEnabled(true);
            stopServerButton->setEnabled(false);
            searchButton->setEnabled(true);
            connectButton->setEnabled(!selectedDevice.isValid() ? false : true);
            disconnectButton->setEnabled(false);
            searchProgressBar->setVisible(false);
            serverStatusLabel->setText("Server stopped");
            clientStatusLabel->setText("Ready to search");
            chatInput->setEnabled(false);
            sendChatButton->setEnabled(false);
            break;
            
        case BluetoothManager::Discovering:
            searchButton->setEnabled(false);
            connectButton->setEnabled(false);
            disconnectButton->setEnabled(false);
            searchProgressBar->setVisible(true);
            clientStatusLabel->setText("Searching for devices...");
            break;
            
        case BluetoothManager::Connecting:
            startServerButton->setEnabled(false);
            searchButton->setEnabled(false);
            connectButton->setEnabled(false);
            disconnectButton->setEnabled(false);
            clientStatusLabel->setText("Connecting...");
            break;
            
        case BluetoothManager::Hosting:
            startServerButton->setEnabled(false);
            stopServerButton->setEnabled(true);
            searchButton->setEnabled(false);
            connectButton->setEnabled(false);
            disconnectButton->setEnabled(true);
            serverStatusLabel->setText("Server running, waiting for connections...");
            break;
            
        case BluetoothManager::Connected:
            startServerButton->setEnabled(false);
            stopServerButton->setEnabled(false);
            searchButton->setEnabled(false);
            connectButton->setEnabled(false);
            disconnectButton->setEnabled(true);
            searchProgressBar->setVisible(false);
            serverStatusLabel->setText("Player connected");
            clientStatusLabel->setText("Connected to host");
            chatInput->setEnabled(true);
            sendChatButton->setEnabled(true);
            break;
    }
}

void BluetoothDialog::onDeviceDiscovered(const QBluetoothDeviceInfo& device)
{
    // Add device to list if not already present
    bool found = false;
    for (const QBluetoothDeviceInfo& existing : availableDevices) {
        if (existing.address() == device.address()) {
            found = true;
            break;
        }
    }
    
    if (!found) {
        availableDevices.append(device);
        
        QString deviceName = device.name();
        if (deviceName.isEmpty()) {
            deviceName = device.address().toString();
        }
        
        QString itemText = QString("%1 (%2)").arg(deviceName, device.address().toString());
        QListWidgetItem* item = new QListWidgetItem(itemText, deviceListWidget);
        
        // Add device type icon or indicator
        if (device.majorDeviceClass() == QBluetoothDeviceInfo::ComputerDevice) {
            item->setText("💻 " + itemText);
        } else if (device.majorDeviceClass() == QBluetoothDeviceInfo::PhoneDevice) {
            item->setText("📱 " + itemText);
        } else {
            item->setText("🔗 " + itemText);
        }
    }
}

void BluetoothDialog::onDiscoveryFinished()
{
    searchProgressBar->setVisible(false);
    clientStatusLabel->setText(QString("Found %1 devices").arg(availableDevices.size()));
}

void BluetoothDialog::onMessageReceived(const GameMessage& message)
{
    if (message.type == GameMessage::CHAT) {
        addChatMessage("Remote Player", message.data);
    }
}

void BluetoothDialog::onErrorOccurred(const QString& error)
{
    QMessageBox::warning(this, "Bluetooth Error", error);
    updateConnectionStatus();
}

void BluetoothDialog::onRemoteDeviceConnected(const QString& deviceName)
{
    remoteDeviceLabel->setText(deviceName);
    addChatMessage("System", QString("Connected to %1").arg(deviceName));
}

void BluetoothDialog::onRemoteDeviceDisconnected()
{
    remoteDeviceLabel->setText("None");
    addChatMessage("System", "Disconnected from remote player");
}

void BluetoothDialog::onBluetoothStateChanged(bool enabled)
{
    enableControls(enabled);
    if (!enabled) {
        QMessageBox::warning(this, "Bluetooth", "Bluetooth has been disabled.");
    }
}

// Private Helper Methods

void BluetoothDialog::updateConnectionStatus()
{
    BluetoothManager::ConnectionState state = bluetoothManager->getConnectionState();
    QString stateText = getConnectionStateText(state);
    
    connectionStatusLabel->setText(stateText);
    
    // Update status color
    QString color;
    switch (state) {
        case BluetoothManager::Connected:
            color = "green";
            break;
        case BluetoothManager::Hosting:
        case BluetoothManager::Connecting:
        case BluetoothManager::Discovering:
            color = "orange";
            break;
        default:
            color = "red";
            break;
    }
    
    connectionStatusLabel->setStyleSheet(QString("QLabel { font-weight: bold; color: %1; }").arg(color));
}

void BluetoothDialog::addChatMessage(const QString& sender, const QString& message)
{
    QString timestamp = QTime::currentTime().toString("hh:mm:ss");
    QString formattedMessage;
    
    if (sender == "System") {
        formattedMessage = QString("<span style='color: #666; font-style: italic;'>[%1] %2</span>").arg(timestamp, message);
    } else if (sender == "You") {
        formattedMessage = QString("<span style='color: #2196F3; font-weight: bold;'>[%1] %2:</span> %3").arg(timestamp, sender, message);
    } else {
        formattedMessage = QString("<span style='color: #4CAF50; font-weight: bold;'>[%1] %2:</span> %3").arg(timestamp, sender, message);
    }
    
    chatDisplay->append(formattedMessage);
    
    // Auto-scroll to bottom
    QScrollBar* scrollBar = chatDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void BluetoothDialog::enableControls(bool enabled)
{
    if (!enabled) {
        startServerButton->setEnabled(false);
        stopServerButton->setEnabled(false);
        searchButton->setEnabled(false);
        connectButton->setEnabled(false);
        disconnectButton->setEnabled(false);
    } else {
        onConnectionStateChanged(bluetoothManager->getConnectionState());
    }
}

QString BluetoothDialog::getConnectionStateText(BluetoothManager::ConnectionState state)
{
    switch (state) {
        case BluetoothManager::Disconnected: return "Disconnected";
        case BluetoothManager::Discovering: return "Searching...";
        case BluetoothManager::Connecting: return "Connecting...";
        case BluetoothManager::Connected: return "Connected";
        case BluetoothManager::Hosting: return "Hosting";
        default: return "Unknown";
    }
}
