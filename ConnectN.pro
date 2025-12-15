QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    board.cpp \
    connectn.cpp \
    connectnfactory.cpp \
    # connectnfactory_old.cpp \
    disk.cpp \
    diskfactory.cpp \
    endofgame.cpp \
    endofround.cpp \
    leaderboard.cpp \
    main.cpp \
    connectnwindow.cpp \
    outoftime.cpp \
    player.cpp \
    startmenu.cpp

HEADERS += \
    board.h \
    connectn.h \
    connectnfactory.h \
    # connectnfactory_old.h \
    connectnwindow.h \
    disk.h \
    diskfactory.h \
    endofgame.h \
    endofround.h \
    leaderboard.h \
    outoftime.h \
    player.h \
    startmenu.h

FORMS += \
    connectnwindow.ui \
    endofgame.ui \
    endofround.ui \
    leaderboard.ui \
    outoftime.ui \
    startmenu.ui

TRANSLATIONS += \
    ConnectN_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    games.qrc
