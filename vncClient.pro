QT += core gui widgets

TARGET = qt5client
TEMPLATE = app

SOURCES += qt5client.cpp

HEADERS += 

# Add any additional include paths here
INCLUDEPATH += ../.. 
INCLUDEPATH += libvnc
INCLUDEPATH += libvnc/rfb
LIBS += -L/home/ubuntu/vncClient/libvnc/lib -lvncclient
