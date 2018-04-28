#ifndef STREAMBROWSER_HPP
#define STREAMBROWSER_HPP

#include "browser.hpp"

namespace Twitch {
class Stream;
}

class StreamWidget;

class StreamBrowser : public Browser {
    Q_OBJECT
public:
    explicit StreamBrowser(QWidget* = nullptr);
signals:
    void streamSelected(const Twitch::Stream&);
    void updateRequested(StreamWidget*);
};

#endif // STREAMBROWSER_HPP
