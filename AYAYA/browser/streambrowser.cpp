#include "streambrowser.hpp"
#include "flowlayout.hpp"
#include "streamwidget.hpp"

StreamBrowser::StreamBrowser(QWidget* parent)
    : Browser(parent)
{
    connect(this, &Browser::itemPressed, [this](BrowserItemWidget* item)
    {
       emit streamSelected(qobject_cast<StreamWidget*>(item)->data().value<Twitch::Stream>());
    });
}
