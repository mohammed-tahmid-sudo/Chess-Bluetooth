#ifndef BLUETOOTHDIALOG_H
#define BLUETOOTHDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QProgressBar>
#include <QTabWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QGroupBox>
#include <QTimer>
#include <QBluetoothDeviceInfo>
#include "bluetoothmanager.h"

class BluetoothDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BluetoothDialog(BluetoothManager* btManager, QWidget *parent = nullptr);
    ~BluetoothDialog();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    // UI event handlers
    void onStartServerClicked();
    void onStopServerClicked();
    void onSearchDevicesClicked();
    void onConnectToDeviceClicked();
    void onDisconnectClicked();
    void onDeviceListItemClicked(QListWidgetItem* item);
    void onSendChatMessage();
    void onRefreshStatus();
    
    // Bluetooth manager event handlers
    void onConnectionStateChanged(BluetoothManager::ConnectionState state);
    void onDeviceDiscovered(const QBluetoothDeviceInfo& device);
    void onDiscoveryFinished();
    void onMessageReceived(const GameMessage& message);
    void onErrorOccurred(const QString& error);
    void onRemoteDeviceConnected(const QString& deviceName);
    void onRemoteDeviceDisconnected();
    void onBluetoothStateChanged(bool enabled);

private:
    void setupUI();
    void setupServerTab();
    void setupClientTab();
    void setupChatTab();
    void updateConnectionStatus();
    void updateDeviceList();
    void addChatMessage(const QString& sender, const QString& message);
    void enableControls(bool enabled);
    QString getConnectionStateText(BluetoothManager::ConnectionState state);
    
    BluetoothManager* bluetoothManager;
    QTimer* statusUpdateTimer;
    
    // UI Components
    QTabWidget* tabWidget;
    
    // Server tab
    QWidget* serverTab;
    QPushButton* startServerButton;
    QPushButton* stopServerButton;
    QLabel* serverStatusLabel;
    QLabel* localDeviceLabel;
    
    // Client tab
    QWidget* clientTab;
    QPushButton* searchButton;
    QPushButton* connectButton;
    QPushButton* disconnectButton;
    QListWidget* deviceListWidget;
    QProgressBar* searchProgressBar;
    QLabel* clientStatusLabel;
    
    // Chat tab
    QWidget* chatTab;
    QTextEdit* chatDisplay;
    QLineEdit* chatInput;
    QPushButton* sendChatButton;
    
    // Status
    QLabel* connectionStatusLabel;
    QLabel* remoteDeviceLabel;
    
    // State
    QList<QBluetoothDeviceInfo> availableDevices;
    QBluetoothDeviceInfo selectedDevice;
};

#endif // BLUETOOTHDIALOG_H
