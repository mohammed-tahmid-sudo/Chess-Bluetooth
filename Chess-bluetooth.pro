QT += core widgets network

# Add Bluetooth support if available
qtHaveModule(bluetooth): QT += bluetooth

CONFIG += c++17

TARGET = ChessBluetoothGame
TEMPLATE = app

# The .cpp file
SOURCES += src/main.cpp

# Additional source files (add as needed)
# SOURCES += src/chessboard.cpp \
#           src/gamelogic.cpp \
#           src/bluetoothmanager.cpp

# Header files (add as needed)  
# HEADERS += src/chessboard.h \
#           src/gamelogic.h \
#           src/bluetoothmanager.h

# UI files (add as needed)
# FORMS += ui/mainwindow.ui

# Resources (add as needed)
RESOURCES += resources/chess.qrc

# Build directories
OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build
UI_DIR = build

# Include paths
INCLUDEPATH += src

# Output directory
DESTDIR = .

# Libraries
unix {
    LIBS += -lbluetooth
}

# Python integration (if needed later)
# LIBS += -lpython3.13

# Compiler flags
QMAKE_CXXFLAGS += -Wall -Wextra -g

# Define for Bluetooth support detection
qtHaveModule(bluetooth) {
    DEFINES += BLUETOOTH_AVAILABLE
}
