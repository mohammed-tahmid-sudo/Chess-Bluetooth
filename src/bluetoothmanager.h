#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include <QObject>
#include <QBluetoothServer>
#include <QBluetoothSocket>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothServiceInfo>
#include <QBluetoothUuid>
#include <QTimer>
#include <QDataStream>
#include <QIODevice>
#include <QString>

struct GameMessage {
    enum Type {
        MOVE,
        NEW_GAME,
        DISCONNECT,
        CHAT,
        GAME_STATE
    };
    
    Type type;
    QString data;
    int fromRow, fromCol, toRow, toCol;
    
    GameMessage() : type(MOVE), fromRow(-1), fromCol(-1), toRow(-1), toCol(-1) {}
    GameMessage(Type t, const QString& d = QString()) : type(t), data(d), fromRow(-1), fromCol(-1), toRow(-1), toCol(-1) {}
    GameMessage(int fr, int fc, int tr, int tc) : type(MOVE), fromRow(fr), fromCol(fc), toRow(tr), toCol(tc) {}
};

Q_DECLARE_METATYPE(GameMessage)

class BluetoothManager : public QObject
{
    Q_OBJECT

public:
    enum ConnectionState {
        Disconnected,
        Discovering,
        Connecting,
        Connected,
        Hosting
    };
    
    explicit BluetoothManager(QObject *parent = nullptr);
    ~BluetoothManager();
    
    // Connection management
    void startServer();
    void stopServer();
    void startDiscovery();
    void connectToDevice(const QBluetoothDeviceInfo& device);
    void disconnect();
    
    // Message sending
    void sendMove(int fromRow, int fromCol, int toRow, int toCol);
    void sendNewGame();
    void sendDisconnect();
    void sendChatMessage(const QString& message);
    
    // Status
    ConnectionState getConnectionState() const { return connectionState; }
    bool isConnected() const { return connectionState == Connected; }
    bool isServer() const { return connectionState == Hosting; }
    QString getRemoteDeviceName() const;
    QList<QBluetoothDeviceInfo> getDiscoveredDevices() const { return discoveredDevices; }
    
    // Settings
    void setLocalDeviceName(const QString& name);
    QString getLocalDeviceName() const;
    bool isBluetoothAvailable() const;

public slots:
    void enableBluetooth();
    void disableBluetooth();

private slots:
    // Server slots
    void onNewConnection();
    void onServerError(QBluetoothServer::Error error);
    
    // Client slots
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QBluetoothSocket::SocketError error);
    void onSocketReadyRead();
    
    // Discovery slots
    void onDeviceDiscovered(const QBluetoothDeviceInfo& device);
    void onDiscoveryFinished();
    void onDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error error);
    
    // Local device slots
    void onLocalDeviceStateChanged(QBluetoothLocalDevice::HostMode mode);
    void onPairingFinished(const QBluetoothAddress& address, QBluetoothLocalDevice::Pairing pairing);

private:
    void setupBluetoothService();
    void sendMessage(const GameMessage& message);
    void processIncomingData();
    void setState(ConnectionState state);
    void cleanup();
    
    // Bluetooth components
    QBluetoothServer* bluetoothServer;
    QBluetoothSocket* bluetoothSocket;
    QBluetoothLocalDevice* localDevice;
    QBluetoothDeviceDiscoveryAgent* discoveryAgent;
    QBluetoothServiceInfo serviceInfo;
    
    // State management
    ConnectionState connectionState;
    QList<QBluetoothDeviceInfo> discoveredDevices;
    
    // Message handling
    QByteArray incomingData;
    quint32 expectedDataSize;
    bool waitingForSize;
    
    // Service info
    static const QString SERVICE_NAME;
    static const QBluetoothUuid SERVICE_UUID;
    static const QString SERVICE_DESCRIPTION;

signals:
    void connectionStateChanged(BluetoothManager::ConnectionState state);
    void deviceDiscovered(const QBluetoothDeviceInfo& device);
    void discoveryFinished();
    void messageReceived(const GameMessage& message);
    void errorOccurred(const QString& error);
    void remoteDeviceConnected(const QString& deviceName);
    void remoteDeviceDisconnected();
    void bluetoothStateChanged(bool enabled);
};

#endif // BLUETOOTHMANAGER_H
