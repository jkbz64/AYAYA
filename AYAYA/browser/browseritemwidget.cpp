#include "browseritemwidget.hpp"
#include <QResizeEvent>

BrowserItemWidget::BrowserItemWidget(QWidget* parent)
    : QFrame(parent)
{
}

void BrowserItemWidget::updateData(const QVariant& data)
{
    m_data.setValue(data);
    emit dataUpdated();
}

const QVariant& BrowserItemWidget::data() const
{
    return m_data;
}

void BrowserItemWidget::mousePressEvent(QMouseEvent* event)
{
    QFrame::mousePressEvent(event);
    emit pressed();
}

void BrowserItemWidget::mouseMoveEvent(QMouseEvent* event)
{
    QFrame::mouseMoveEvent(event);
    emit hovered();
}

void BrowserItemWidget::resizeEvent(QResizeEvent* event)
{
    QFrame::resizeEvent(event);
    emit resized(event->oldSize(), event->size());
}
