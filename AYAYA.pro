QT       += core gui widgets network
CONFIG   += c++11 communi

COMMUNI  += core model util

TARGET = AYAYA
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

# TwitchQt
include(lib/TwitchQt/TwitchQt.pri)
# libcommuni
include(lib/libcommuni/src/src.pri)

RESOURCES += lib/QDarkStyleSheet/qdarkstyle/style.qrc \

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
		AYAYA/player/mpvcontroller.cpp \
		AYAYA/player/mpvplayer.cpp \
		AYAYA/player/player.cpp \
		AYAYA/player/controlswidget.cpp \
    AYAYA/browser/browser.cpp \
    AYAYA/browser/browseritemwidget.cpp \
    AYAYA/chat/chatserver.cpp

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
	AYAYA/player/mpvcontroller.hpp \
	AYAYA/player/mpvplayer.hpp \
	AYAYA/player/player.hpp \
	AYAYA/player/playercontroller.hpp \
	AYAYA/player/controlswidget.hpp \
    AYAYA/browser/browser.hpp \
    AYAYA/browser/browseritemwidget.hpp \
    AYAYA/chat/chatserver.hpp

FORMS += \
	AYAYA/mainwindow.ui \
	AYAYA/livestreamwidget.ui \
	AYAYA/browserwidget.ui \
	AYAYA/player/controlswidget.ui

LIBS += -lmpv
