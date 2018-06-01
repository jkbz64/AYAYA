#ifndef STREAMITEMWIDGET_HPP
#define STREAMITEMWIDGET_HPP

#include "browseritemwidget.hpp"
#include <TwitchQt/twitchstream.hpp>

namespace Ui {
class StreamItemWidget;
}

class StreamItemWidget : public BrowserItemWidget {
    Q_OBJECT
public:
    explicit StreamItemWidget(const Twitch::Stream&, QWidget*);
    ~StreamItemWidget();

    const Twitch::Stream& stream() const;

    void setPreview(const QPixmap&);
signals:
    void hovered();
    void pressed();

private:
    Ui::StreamItemWidget* m_ui;
    const Twitch::Stream m_stream;
};

#endif // STREAMWIDGET_HPP
