#include "streamitemwidget.hpp"
#include <QPainter>
#include <TwitchQt/twitchstream.hpp>

StreamItemWidget::StreamItemWidget(const Twitch::Stream& stream)
    : BrowserItemWidget(nullptr)
{
    m_data.setValue(stream);

    setFrameStyle(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    setLineWidth(10);
}

StreamItemWidget::StreamItemWidget(const Twitch::Stream& stream, QWidget* parent)
    : BrowserItemWidget(parent)
{
    m_data.setValue(stream);
}

void StreamItemWidget::setPreview(const QPixmap& preview)
{
    m_preview = preview;
}

void StreamItemWidget::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);
    QPainter painter(this);
    if (!m_preview.isNull())
        painter.drawPixmap(rect(), m_preview);

    painter.setPen(Qt::black);
    painter.setFont(QFont("DIMITRI", 15));
    painter.drawText(rect(), Qt::AlignCenter, "TODO");
}
