QT       += core gui widgets network
CONFIG   += c++17 communi

COMMUNI  += core model util

TARGET = AYAYA
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

# TwitchQt
include(lib/TwitchQt/TwitchQt.pri)
# libcommuni
include(lib/libcommuni/src/src.pri)

RESOURCES += AYAYA/fx/ayaya.qrc \
	lib/QDarkStyleSheet/qdarkstyle/style.qrc \


INCLUDEPATH += AYAYA/ \
			   lib/libcommuni/include/ \

SOURCES += \
		AYAYA/main.cpp \
		AYAYA/mainwindow.cpp \
		AYAYA/livestreamwidget.cpp \
		AYAYA/browserwidget.cpp \
		AYAYA/browser/flowlayout.cpp \
		AYAYA/browser/gamewidget.cpp \
		AYAYA/browser/streamwidget.cpp \
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
    AYAYA/player/playerimpl.cpp

HEADERS += \
	AYAYA/mainwindow.hpp \
	AYAYA/livestreamwidget.hpp \
	AYAYA/browserwidget.hpp \
	AYAYA/browser/flowlayout.hpp \
	AYAYA/browser/gamewidget.hpp \
	AYAYA/browser/streamwidget.hpp \
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
    AYAYA/player/playerimpl.hpp

FORMS += \
	AYAYA/mainwindow.ui \
	AYAYA/livestreamwidget.ui \
	AYAYA/browserwidget.ui \
	AYAYA/player/controlswidget.ui \
        AYAYA/chat/chatwidget.ui

LIBS += -lmpv
