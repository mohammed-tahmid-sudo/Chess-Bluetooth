######################################################################
# Chess-bluetooth - Qt Chess Game with Bluetooth Support
######################################################################

QT += widgets bluetooth
TEMPLATE = app
TARGET = Chess-bluetooth
INCLUDEPATH += . src

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_UP_TO=0x060000 # disables all APIs deprecated in Qt 6.0.0 and earlier

# Input
SOURCES += src/main.cpp \
           src/mainwindow.cpp \
           src/chessboard.cpp \
           src/chessai.cpp \
           src/bluetoothmanager.cpp \
           src/bluetoothdialog.cpp

HEADERS += src/mainwindow.h \
           src/chessboard.h \
           src/chessai.h \
           src/bluetoothmanager.h \
           src/bluetoothdialog.h

RESOURCES += resources/chess.qrc
