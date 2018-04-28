#include "streamwidget.hpp"
#include <QPainter>

StreamWidget::StreamWidget(const Twitch::Stream& stream)
    : BrowserItemWidget(nullptr)
{
    m_data.setValue(stream);

    setFrameStyle(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    setLineWidth(10);
}

StreamWidget::StreamWidget(const Twitch::Stream& stream, QWidget* parent)
    : BrowserItemWidget(parent)
{
    m_data.setValue(stream);
}

void StreamWidget::setPreview(const QPixmap& preview)
{
    m_preview = preview;
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
