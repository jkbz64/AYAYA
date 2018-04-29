#include "streambrowser.hpp"
#include "flowlayout.hpp"
#include "streamwidget.hpp"

#include <TwitchQt/twitchstream.hpp>

StreamBrowser::StreamBrowser(QWidget* parent)
    : Browser(parent)
{
    connect(this, &Browser::itemPressed, [this](BrowserItemWidget* item) {
        emit streamSelected(qobject_cast<StreamWidget*>(item)->data().value<Twitch::Stream>());
    });
}
