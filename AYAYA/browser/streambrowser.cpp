#include "streambrowser.hpp"
#include "flowlayout.hpp"
#include "streamwidget.hpp"

StreamBrowser::StreamBrowser(QWidget* parent)
    : QScrollArea(parent)
{
    setWidgetResizable(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    auto central = new QWidget(this);
    auto lay = new FlowLayout(this);

    central->setLayout(lay);
    setWidget(central);
}

StreamWidget* StreamBrowser::addStream(const Twitch::Stream& stream)
{
    auto streamWidget = new StreamWidget(stream, this);
    streamWidget->setFixedWidth(150);
    streamWidget->setFixedHeight(150);
    streamWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    widget()->layout()->addWidget(streamWidget);

    connect(streamWidget, &StreamWidget::pressed, [this, streamWidget]() {
        emit streamSelected(streamWidget->data());
    });

    emit updateRequested(streamWidget);
    m_visibleWidgets.push_back(streamWidget);
    return streamWidget;
}

void StreamBrowser::clear()
{
    for (auto& widget : m_visibleWidgets)
        widget->deleteLater();
    m_visibleWidgets.clear();
    widget()->layout()->update();
}
