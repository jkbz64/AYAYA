#include "streamwidget.hpp"
#include <QPainter>

StreamWidget::StreamWidget(const Twitch::Stream& stream, QWidget* parent)
    : QFrame(parent)
    , m_data(stream)
{
    setFrameStyle(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    setLineWidth(10);
}

void StreamWidget::setPreview(const QPixmap& preview)
{
    m_preview = preview;
}

const Twitch::Stream& StreamWidget::data() const
{
    return m_data;
}

void StreamWidget::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);
    QPainter painter(this);
    if (!m_preview.isNull())
        painter.drawPixmap(rect(), m_preview);

    painter.setPen(Qt::black);
    painter.setFont(QFont("DIMITRI", 15));
    painter.drawText(rect(), Qt::AlignCenter, "TODO");
}

void StreamWidget::mouseMoveEvent(QMouseEvent* event)
{
    emit hovered();
}

void StreamWidget::mousePressEvent(QMouseEvent* event)
{
    emit pressed();
}
