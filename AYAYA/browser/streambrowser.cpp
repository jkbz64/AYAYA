#include "streambrowser.hpp"
#include "streamitemwidget.hpp"
#include <QVBoxLayout>

StreamBrowser::StreamBrowser(QWidget* parent)
    : Browser(parent)
{
    centralWidget()->setLayout(new QVBoxLayout);
}

StreamBrowser::~StreamBrowser()
{
}

StreamItemWidget* StreamBrowser::addStream(const Twitch::Stream& stream)
{
    auto streamWidget = new StreamItemWidget(stream, centralWidget());
    streamWidget->setFixedHeight(150);
    centralWidget()->layout()->addWidget(streamWidget);

    addItem(streamWidget);
    return streamWidget;
}
