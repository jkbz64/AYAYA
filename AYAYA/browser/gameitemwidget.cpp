#include "gameitemwidget.hpp"
#include "ui_gameitemwidget.h"

#include <QGraphicsOpacityEffect>

#include <QPropertyAnimation>

GameItemWidget::GameItemWidget(const Twitch::Game& data, QWidget* parent)
    : BrowserItemWidget(parent)
    , m_ui(new Ui::GameItemWidget)
    , m_game(data)
    , m_viewerCount(0)
    , m_highlightAnimation(new QPropertyAnimation(this, "color"))
    , m_currentColor(palette().background().color())
{
    m_ui->setupUi(this);
    m_ui->m_nameLabel->setText(game().m_name);

    auto opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(0.99);
    setGraphicsEffect(opacityEffect);

    setAutoFillBackground(true);

    setMouseTracking(true);
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

void GameItemWidget::setColor(QColor color)
{
    m_currentColor = color;
    setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
}

const QColor& GameItemWidget::color()
{
    return m_currentColor;
}

void GameItemWidget::enterEvent(QEvent*)
{
    m_highlightAnimation->setDuration(200);
    m_highlightAnimation->setStartValue(QColor(255, 255, 255, 255));
    m_highlightAnimation->setEndValue(QColor(100, 65, 164));
    m_highlightAnimation->start();
}

void GameItemWidget::leaveEvent(QEvent*)
{
    m_highlightAnimation->stop();
    setStyleSheet("");
}
