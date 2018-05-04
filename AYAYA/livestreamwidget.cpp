#include "livestreamwidget.hpp"
#include "ui_livestreamwidget.h"

#include <TwitchQt/twitchstream.hpp>
#include <TwitchQt/twitchuser.hpp>

LivestreamWidget::LivestreamWidget(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::LivestreamWidget)
{
    m_ui->setupUi(this);
}

LivestreamWidget::~LivestreamWidget()
{
    delete m_ui;
}

void LivestreamWidget::initialize(const Twitch::User& user, const Twitch::Stream& stream)
{
    //m_ui->m_player->openStream(user.m_login);
    m_ui->m_chat->openChat(user);
}
