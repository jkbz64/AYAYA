#include "streamitemwidget.hpp"
#include "ui_streamitemwidget.h"

StreamItemWidget::StreamItemWidget(const Twitch::Stream& stream, QWidget* parent)
    : BrowserItemWidget(parent)
    , m_ui(new Ui::StreamItemWidget)
    , m_stream(stream)
{
    m_ui->setupUi(this);
    m_ui->m_nameLabel->setText(stream.m_userName);
    m_ui->m_titleLabel->setText(stream.m_title);
}

StreamItemWidget::~StreamItemWidget()
{
    delete m_ui;
}

const Twitch::Stream& StreamItemWidget::stream() const
{
    return m_stream;
}

void StreamItemWidget::setPreview(const QPixmap&)
{
}
