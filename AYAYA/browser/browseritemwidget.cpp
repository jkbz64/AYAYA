#include "browseritemwidget.hpp"

BrowserItemWidget::BrowserItemWidget(QWidget* parent)
    : QFrame(parent)
{
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
