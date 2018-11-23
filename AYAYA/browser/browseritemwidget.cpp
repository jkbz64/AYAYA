#include "browseritemwidget.hpp"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

BrowserItemWidget::BrowserItemWidget(QWidget* parent)
    : QFrame(parent)
    , m_highlightAnimation(new QPropertyAnimation(this, "color"))
    , m_currentColor(palette().background().color())
{
    auto opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(0.99);
    setGraphicsEffect(opacityEffect);

    setAutoFillBackground(true);

    setMouseTracking(true);
}

BrowserItemWidget::~BrowserItemWidget() = default;

void BrowserItemWidget::mousePressEvent(QMouseEvent* event)
{
    emit pressed();
    QFrame::mousePressEvent(event);
}

void BrowserItemWidget::mouseMoveEvent(QMouseEvent* event)
{
    emit hovered();
    QFrame::mouseMoveEvent(event);
}

void BrowserItemWidget::setColor(QColor color)
{
    m_currentColor = color;
    setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
}

const QColor& BrowserItemWidget::color()
{
    return m_currentColor;
}

void BrowserItemWidget::enterEvent(QEvent*)
{
    m_highlightAnimation->setDuration(200);
    m_highlightAnimation->setStartValue(QColor(255, 255, 255, 255));
    m_highlightAnimation->setEndValue(QColor(100, 65, 164));
    m_highlightAnimation->start();
}

void BrowserItemWidget::leaveEvent(QEvent*)
{
    m_highlightAnimation->stop();
    setStyleSheet("");
}
