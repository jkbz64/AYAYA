QT       += core gui widgets network
CONFIG   += c++14 communi

COMMUNI  += core model util

TARGET = AYAYA
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

# TwitchQt
include(lib/TwitchQt/TwitchQt.pri)
# libcommuni
include(lib/libcommuni/src/src.pri)

RESOURCES += \
    AYAYA/fx/ayaya.qrc \
    lib/QDarkStyleSheet/qdarkstyle/style.qrc \

INCLUDEPATH += \
    lib/libcommuni/include/ \

SOURCES += \
    AYAYA/main.cpp \
    AYAYA/mainwindow.cpp \
    AYAYA/browserwidget.cpp \
    AYAYA/browser/flowlayout.cpp \
    AYAYA/browser/gamebrowser.cpp \
    AYAYA/browser/streambrowser.cpp \
    AYAYA/chat/chatwidget.cpp \
    AYAYA/player/playerwidget.cpp \
    AYAYA/player/controlswidget.cpp \
    AYAYA/browser/browser.cpp \
    AYAYA/browser/browseritemwidget.cpp \
    AYAYA/chat/chatview.cpp \
    AYAYA/chat/chatclient.cpp \
    AYAYA/player/mpvplayerimpl.cpp \
    AYAYA/player/playerimpl.cpp \
    AYAYA/browser/streamitemwidget.cpp \
    AYAYA/browser/gameitemwidget.cpp \
    AYAYA/streamwidget.cpp \
    AYAYA/chat/emotescache.cpp \
    AYAYA/mainwidget.cpp

HEADERS += \
    AYAYA/mainwindow.hpp \
    AYAYA/browserwidget.hpp \
    AYAYA/browser/flowlayout.hpp \
    AYAYA/browser/gamebrowser.hpp \
    AYAYA/browser/streambrowser.hpp \
    AYAYA/chat/chatwidget.hpp \
    AYAYA/player/playerwidget.hpp \
    AYAYA/player/controlswidget.hpp \
    AYAYA/browser/browser.hpp \
    AYAYA/browser/browseritemwidget.hpp \
    AYAYA/chat/chatview.hpp \
    AYAYA/chat/chatclient.hpp \
    AYAYA/player/mpvplayerimpl.hpp \
    AYAYA/player/playerimpl.hpp \
    AYAYA/browser/streamitemwidget.hpp \
    AYAYA/browser/gameitemwidget.hpp \
    AYAYA/streamwidget.hpp \
    AYAYA/chat/emotescache.hpp \
    AYAYA/mainwidget.hpp

FORMS += \
    AYAYA/mainwindow.ui \
    AYAYA/browserwidget.ui \
    AYAYA/player/controlswidget.ui \
    AYAYA/chat/chatwidget.ui \
    AYAYA/streamwidget.ui

LIBS += -lmpv
