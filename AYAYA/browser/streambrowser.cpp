#include "streambrowser.hpp"
#include "flowlayout.hpp"
#include "streamitemwidget.hpp"

#include <TwitchQt/twitchstream.hpp>

StreamBrowser::StreamBrowser(QWidget* parent)
    : Browser(parent)
{
    connect(this, &Browser::itemPressed, [this](BrowserItemWidget* item) {
        emit streamSelected(qobject_cast<StreamItemWidget*>(item)->data().value<Twitch::Stream>());
    });
}
