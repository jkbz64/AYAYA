#ifndef STREAMBROWSER_HPP
#define STREAMBROWSER_HPP

#include "browser.hpp"

class StreamItemWidget;

namespace Twitch {
class Stream;
}

class StreamBrowser : public Browser {
    Q_OBJECT
public:
    explicit StreamBrowser(QWidget* parent = nullptr);
    ~StreamBrowser();

    StreamItemWidget* addStream(const Twitch::Stream&);
};

#endif // STREAMBROWSER_HPP
