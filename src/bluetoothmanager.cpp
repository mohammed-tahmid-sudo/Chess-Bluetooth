#include "bluetoothmanager.h"
#include <QDebug>
#include <QCoreApplication>

// Static service constants
const QString BluetoothManager::SERVICE_NAME = "Chess Game";
const QBluetoothUuid BluetoothManager::SERVICE_UUID = QBluetoothUuid(QString("12345678-1234-5678-9abc-123456789abc"));
const QString BluetoothManager::SERVICE_DESCRIPTION = "Bluetooth Chess Game Service";

BluetoothManager::BluetoothManager(QObject *parent)
    : QObject(parent)
    , bluetoothServer(nullptr)
    , bluetoothSocket(nullptr)
    , localDevice(nullptr)
    , discoveryAgent(nullptr)
    , connectionState(Disconnected)
    , expectedDataSize(0)
    , waitingForSize(true)
{
    // Register the custom type for signals/slots
    qRegisterMetaType<GameMessage>("GameMessage");
    qRegisterMetaType<BluetoothManager::ConnectionState>("BluetoothManager::ConnectionState");
    
    // Initialize local Bluetooth device
    localDevice = new QBluetoothLocalDevice(this);
    
    // Check if Bluetooth is available
    if (!localDevice->isValid()) {
        qWarning() << "No valid Bluetooth adapter found";
        return;
    }
    
    // Connect local device signals
    connect(localDevice, &QBluetoothLocalDevice::hostModeStateChanged,
            this, &BluetoothManager::onLocalDeviceStateChanged);
    connect(localDevice, &QBluetoothLocalDevice::pairingFinished,
            this, &BluetoothManager::onPairingFinished);
    
    // Initialize discovery agent
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &BluetoothManager::onDeviceDiscovered);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &BluetoothManager::onDiscoveryFinished);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred,
            this, &BluetoothManager::onDiscoveryError);
}

BluetoothManager::~BluetoothManager()
{
    cleanup();
}

void BluetoothManager::startServer()
{
    if (!isBluetoothAvailable()) {
        emit errorOccurred("Bluetooth is not available");
        return;
    }
    
    if (connectionState == Hosting || connectionState == Connected) {
        return; // Already hosting or connected
    }
    
    cleanup();
    
    bluetoothServer = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
    connect(bluetoothServer, &QBluetoothServer::newConnection,
            this, &BluetoothManager::onNewConnection);
    connect(bluetoothServer, &QBluetoothServer::errorOccurred,
            this, &BluetoothManager::onServerError);
    
    bool result = bluetoothServer->listen(localDevice->address());
    if (!result) {
        emit errorOccurred("Failed to start Bluetooth server");
        return;
    }
    
    setupBluetoothService();
    setState(Hosting);
    
    qDebug() << "Bluetooth server started, waiting for connections...";
}

void BluetoothManager::stopServer()
{
    if (bluetoothServer) {
        bluetoothServer->close();
        delete bluetoothServer;
        bluetoothServer = nullptr;
    }
    
    serviceInfo.unregisterService();
    setState(Disconnected);
}

void BluetoothManager::startDiscovery()
{
    if (!isBluetoothAvailable()) {
        emit errorOccurred("Bluetooth is not available");
        return;
    }
    
    if (discoveryAgent->isActive()) {
        discoveryAgent->stop();
    }
    
    discoveredDevices.clear();
    setState(Discovering);
    
    discoveryAgent->start();
    qDebug() << "Started device discovery";
}

void BluetoothManager::connectToDevice(const QBluetoothDeviceInfo& device)
{
    if (connectionState == Connected || connectionState == Connecting) {
        return; // Already connected or connecting
    }
    
    cleanup();
    
    setState(Connecting);
    
    bluetoothSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
    connect(bluetoothSocket, &QBluetoothSocket::connected,
            this, &BluetoothManager::onSocketConnected);
    connect(bluetoothSocket, &QBluetoothSocket::disconnected,
            this, &BluetoothManager::onSocketDisconnected);
    connect(bluetoothSocket, QOverload<QBluetoothSocket::SocketError>::of(&QBluetoothSocket::errorOccurred),
            this, &BluetoothManager::onSocketError);
    connect(bluetoothSocket, &QBluetoothSocket::readyRead,
            this, &BluetoothManager::onSocketReadyRead);
    
    qDebug() << "Connecting to device:" << device.name() << device.address().toString();
    
    // Connect to the chess service
    bluetoothSocket->connectToService(device.address(), SERVICE_UUID);
}

void BluetoothManager::disconnect()
{
    sendDisconnect();
    cleanup();
    setState(Disconnected);
}

void BluetoothManager::sendMove(int fromRow, int fromCol, int toRow, int toCol)
{
    if (!isConnected()) return;
    
    GameMessage message(fromRow, fromCol, toRow, toCol);
    sendMessage(message);
}

void BluetoothManager::sendNewGame()
{
    if (!isConnected()) return;
    
    GameMessage message(GameMessage::NEW_GAME);
    sendMessage(message);
}

void BluetoothManager::sendDisconnect()
{
    if (!isConnected()) return;
    
    GameMessage message(GameMessage::DISCONNECT);
    sendMessage(message);
}

void BluetoothManager::sendChatMessage(const QString& message)
{
    if (!isConnected()) return;
    
    GameMessage gameMessage(GameMessage::CHAT, message);
    sendMessage(gameMessage);
}

QString BluetoothManager::getRemoteDeviceName() const
{
    if (bluetoothSocket && bluetoothSocket->peerName().isEmpty()) {
        return bluetoothSocket->peerAddress().toString();
    }
    return bluetoothSocket ? bluetoothSocket->peerName() : QString();
}

void BluetoothManager::setLocalDeviceName(const QString& name)
{
    // Note: In Qt6, QBluetoothLocalDevice doesn't have setName method
    // The device name is typically set at system level
    Q_UNUSED(name)
    qDebug() << "Setting device name is not supported in Qt6";
}

QString BluetoothManager::getLocalDeviceName() const
{
    return localDevice ? localDevice->name() : QString();
}

bool BluetoothManager::isBluetoothAvailable() const
{
    return localDevice && localDevice->isValid() && 
           localDevice->hostMode() != QBluetoothLocalDevice::HostPoweredOff;
}

void BluetoothManager::enableBluetooth()
{
    if (localDevice) {
        localDevice->setHostMode(QBluetoothLocalDevice::HostDiscoverable);
    }
}

void BluetoothManager::disableBluetooth()
{
    if (localDevice) {
        cleanup();
        localDevice->setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    }
}

// Private slots implementation

void BluetoothManager::onNewConnection()
{
    if (!bluetoothServer) return;
    
    QBluetoothSocket* socket = bluetoothServer->nextPendingConnection();
    if (!socket) return;
    
    // Accept only one connection
    if (bluetoothSocket) {
        socket->deleteLater();
        return;
    }
    
    bluetoothSocket = socket;
    connect(bluetoothSocket, &QBluetoothSocket::disconnected,
            this, &BluetoothManager::onSocketDisconnected);
    connect(bluetoothSocket, QOverload<QBluetoothSocket::SocketError>::of(&QBluetoothSocket::errorOccurred),
            this, &BluetoothManager::onSocketError);
    connect(bluetoothSocket, &QBluetoothSocket::readyRead,
            this, &BluetoothManager::onSocketReadyRead);
    
    setState(Connected);
    emit remoteDeviceConnected(getRemoteDeviceName());
    
    qDebug() << "Client connected:" << getRemoteDeviceName();
}

void BluetoothManager::onServerError(QBluetoothServer::Error error)
{
    Q_UNUSED(error)
    emit errorOccurred("Server error occurred");
}

void BluetoothManager::onSocketConnected()
{
    setState(Connected);
    emit remoteDeviceConnected(getRemoteDeviceName());
    
    qDebug() << "Connected to remote device:" << getRemoteDeviceName();
}

void BluetoothManager::onSocketDisconnected()
{
    emit remoteDeviceDisconnected();
    setState(Disconnected);
    
    if (bluetoothSocket) {
        bluetoothSocket->deleteLater();
        bluetoothSocket = nullptr;
    }
    
    qDebug() << "Disconnected from remote device";
}

void BluetoothManager::onSocketError(QBluetoothSocket::SocketError error)
{
    Q_UNUSED(error)
    QString errorMessage = bluetoothSocket ? bluetoothSocket->errorString() : "Unknown socket error";
    emit errorOccurred("Connection error: " + errorMessage);
    
    setState(Disconnected);
}

void BluetoothManager::onSocketReadyRead()
{
    if (!bluetoothSocket) return;
    
    incomingData.append(bluetoothSocket->readAll());
    processIncomingData();
}

void BluetoothManager::onDeviceDiscovered(const QBluetoothDeviceInfo& device)
{
    // Only add devices that aren't already in the list
    bool found = false;
    for (const QBluetoothDeviceInfo& existingDevice : discoveredDevices) {
        if (existingDevice.address() == device.address()) {
            found = true;
            break;
        }
    }
    
    if (!found) {
        discoveredDevices.append(device);
        emit deviceDiscovered(device);
        
        qDebug() << "Device discovered:" << device.name() << device.address().toString();
    }
}

void BluetoothManager::onDiscoveryFinished()
{
    setState(Disconnected);
    emit discoveryFinished();
    
    qDebug() << "Device discovery finished. Found" << discoveredDevices.size() << "devices";
}

void BluetoothManager::onDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    Q_UNUSED(error)
    emit errorOccurred("Device discovery failed");
    setState(Disconnected);
}

void BluetoothManager::onLocalDeviceStateChanged(QBluetoothLocalDevice::HostMode mode)
{
    bool enabled = (mode != QBluetoothLocalDevice::HostPoweredOff);
    emit bluetoothStateChanged(enabled);
    
    if (!enabled) {
        cleanup();
        setState(Disconnected);
    }
}

void BluetoothManager::onPairingFinished(const QBluetoothAddress& address, QBluetoothLocalDevice::Pairing pairing)
{
    Q_UNUSED(address)
    Q_UNUSED(pairing)
    // Handle pairing completion if needed
}

// Private methods implementation

void BluetoothManager::setupBluetoothService()
{
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, SERVICE_NAME);
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription, SERVICE_DESCRIPTION);
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, "Chess Game App");
    
    serviceInfo.setServiceUuid(SERVICE_UUID);
    
    QBluetoothServiceInfo::Sequence protocolDescriptorList;
    QBluetoothServiceInfo::Sequence protocol;
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::ProtocolUuid::Rfcomm))
             << QVariant::fromValue(quint8(bluetoothServer->serverPort()));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList, protocolDescriptorList);
    
    serviceInfo.registerService(localDevice->address());
}

void BluetoothManager::sendMessage(const GameMessage& message)
{
    if (!bluetoothSocket || bluetoothSocket->state() != QBluetoothSocket::SocketState::ConnectedState) {
        return;
    }
    
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_15);
    
    // Serialize the message
    stream << static_cast<quint32>(message.type);
    stream << message.data;
    stream << static_cast<qint32>(message.fromRow);
    stream << static_cast<qint32>(message.fromCol);
    stream << static_cast<qint32>(message.toRow);
    stream << static_cast<qint32>(message.toCol);
    
    // Send size first, then data
    QByteArray sizeData;
    QDataStream sizeStream(&sizeData, QIODevice::WriteOnly);
    sizeStream.setVersion(QDataStream::Qt_5_15);
    sizeStream << static_cast<quint32>(data.size());
    
    bluetoothSocket->write(sizeData);
    bluetoothSocket->write(data);
}

void BluetoothManager::processIncomingData()
{
    QDataStream stream(&incomingData, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_5_15);
    
    while (true) {
        if (waitingForSize) {
            if (incomingData.size() < sizeof(quint32)) {
                break; // Need more data for size
            }
            
            stream >> expectedDataSize;
            waitingForSize = false;
            incomingData.remove(0, sizeof(quint32));
            stream.device()->seek(0);
        }
        
        if (incomingData.size() < expectedDataSize) {
            break; // Need more data for complete message
        }
        
        // Read the complete message
        GameMessage message;
        quint32 type;
        qint32 fromRow, fromCol, toRow, toCol;
        
        stream >> type >> message.data >> fromRow >> fromCol >> toRow >> toCol;
        
        message.type = static_cast<GameMessage::Type>(type);
        message.fromRow = fromRow;
        message.fromCol = fromCol;
        message.toRow = toRow;
        message.toCol = toCol;
        
        emit messageReceived(message);
        
        // Remove processed data and reset for next message
        incomingData.remove(0, expectedDataSize);
        stream.device()->seek(0);
        waitingForSize = true;
        expectedDataSize = 0;
    }
}

void BluetoothManager::setState(ConnectionState state)
{
    if (connectionState != state) {
        connectionState = state;
        emit connectionStateChanged(state);
    }
}

void BluetoothManager::cleanup()
{
    if (bluetoothSocket) {
        bluetoothSocket->disconnectFromService();
        bluetoothSocket->deleteLater();
        bluetoothSocket = nullptr;
    }
    
    if (bluetoothServer) {
        bluetoothServer->close();
        delete bluetoothServer;
        bluetoothServer = nullptr;
    }
    
    serviceInfo.unregisterService();
    
    if (discoveryAgent && discoveryAgent->isActive()) {
        discoveryAgent->stop();
    }
    
    discoveredDevices.clear();
    incomingData.clear();
    expectedDataSize = 0;
    waitingForSize = true;
}
