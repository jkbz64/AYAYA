#include "gamewidget.hpp"
#include <QPaintEvent>
#include <QPainter>

GameWidget::GameWidget(const Twitch::Game& game, QWidget* parent = nullptr)
    : QFrame(parent)
    , m_data(game)
    , m_boxArt()
    , m_viewerCount(0)
{
    setFrameStyle(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    setLineWidth(10);
    setMouseTracking(true);
}

void GameWidget::setBoxArt(const QPixmap& boxArt)
{
    m_boxArt = boxArt;
    update();
}

void GameWidget::setViewerCount(int viewerCount)
{
    m_viewerCount = viewerCount;
    update();
}

const QPixmap& GameWidget::boxArt() const
{
    return m_boxArt;
}

const Twitch::Game& GameWidget::data() const
{
    return m_data;
}

void GameWidget::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);
    QPainter painter(this);
    auto r = rect();
    if (!m_boxArt.isNull()) {
        r.adjust(5, 5, -5, -5);
        painter.drawPixmap(r, m_boxArt);
    } else {
        painter.setPen(Qt::black);
        painter.setFont(QFont("DIMITRI", 15));
        painter.drawText(rect(), Qt::AlignCenter, m_data.m_name);
    }

    // TODO
    /*
    painter.setPen(Qt::red);
    painter.setFont(QFont("DIMITRI", 15));
    r.setHeight(50);
    painter.drawText(r, Qt::AlignRight, QString::number(m_viewerCount) + "+");
    */
}

void GameWidget::mouseMoveEvent(QMouseEvent* event)
{
    emit hovered();
}

void GameWidget::mousePressEvent(QMouseEvent* event)
{
    emit pressed();
}
