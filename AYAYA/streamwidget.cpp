#include "streamwidget.hpp"
#include "ui_streamwidget.h"

#include <TwitchQt/twitchstream.hpp>
#include <TwitchQt/twitchuser.hpp>

#include "player/controlswidget.hpp"

StreamWidget::StreamWidget(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::StreamWidget)
    , m_currentMode(WatchMode::Normal)
{
    m_ui->setupUi(this);

    connect(m_ui->m_player->controlsWidget(), &ControlsWidget::pressedTheaterButton, this, &StreamWidget::onPressedTheaterButton);
    connect(m_ui->m_player->controlsWidget(), &ControlsWidget::pressedFullscreenButton, this, &StreamWidget::onPressedFullscreenButton);

    connect(this, &StreamWidget::enteredTheaterMode, m_ui->m_player, &PlayerWidget::resetStream);
    connect(this, &StreamWidget::enteredFullscreenMode, m_ui->m_player, &PlayerWidget::resetStream);
    connect(this, &StreamWidget::leftTheaterMode, m_ui->m_player, &PlayerWidget::resetStream);
    connect(this, &StreamWidget::leftFullscreenMode, m_ui->m_player, &PlayerWidget::resetStream);

    connect(m_ui->splitter, &QSplitter::splitterMoved, this, &StreamWidget::onSplitterMoved);
}

StreamWidget::~StreamWidget()
{
    delete m_ui;
}

void StreamWidget::initialize(const Twitch::User& user, const Twitch::Stream& stream)
{
    m_ui->m_player->openStream(user.m_login);
    m_ui->m_chat->openChat(user);
}

void StreamWidget::onSplitterMoved()
{
    m_ui->m_chat->followChat();
}

void StreamWidget::onPressedTheaterButton()
{
    if (m_currentMode != WatchMode::Theater) {
        m_currentMode = WatchMode::Theater;
        setWindowTitle("Theater Mode");
        setWindowFlags(Qt::Window);
        showMaximized();
        emit enteredTheaterMode();
    } else {
        m_currentMode = WatchMode::Normal;
        setWindowFlags(Qt::Widget);
        emit leftTheaterMode();
    }
}

void StreamWidget::onPressedFullscreenButton()
{
    if (m_currentMode != WatchMode::Fullscreen) {
        m_currentMode = WatchMode::Fullscreen;
        setWindowTitle("AYAYA");
        setWindowFlags(Qt::Window);
        showFullScreen();
        emit enteredFullscreenMode();
    } else {
        m_currentMode = WatchMode::Normal;
        setWindowFlags(Qt::Widget);
        emit leftFullscreenMode();
    }
}
