#include "gameitemwidget.hpp"
#include "ui_gameitemwidget.h"

#include <QGraphicsOpacityEffect>

#include <QPropertyAnimation>

GameItemWidget::GameItemWidget(const Twitch::Game& data, QWidget* parent)
    : BrowserItemWidget(parent)
    , m_ui(new Ui::GameItemWidget)
    , m_game(data)
    , m_viewerCount(0)
{
    m_ui->setupUi(this);
    m_ui->m_nameLabel->setText(game().m_name);
}

GameItemWidget::~GameItemWidget()
{
    delete m_ui;
}

const Twitch::Game& GameItemWidget::game() const
{
    return m_game;
}

void GameItemWidget::setBoxArt(const QPixmap& boxArt)
{
    m_ui->m_boxArtLabel->setPixmap(boxArt);
}

void GameItemWidget::setViewerCount(int viewerCount)
{
    m_viewerCount = viewerCount;
    update();
}

const QPixmap& GameItemWidget::boxArt() const
{
    if (m_ui->m_boxArtLabel->pixmap())
        return *m_ui->m_nameLabel->pixmap();
    else {
        static QPixmap nullPixmap;
        return nullPixmap;
    }
}
