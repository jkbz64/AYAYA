#ifndef STREAMITEMWIDGET_HPP
#define STREAMITEMWIDGET_HPP

#include "browseritemwidget.hpp"

namespace Twitch {
class Stream;
}

class StreamItemWidget : public BrowserItemWidget {
    Q_OBJECT
public:
    explicit StreamItemWidget(const Twitch::Stream&);
    StreamItemWidget(const Twitch::Stream&, QWidget*);

    void setPreview(const QPixmap&);
signals:
    void hovered();
    void pressed();

protected:
    virtual void paintEvent(QPaintEvent*) override;

private:
    QPixmap m_preview;
};

#endif // STREAMWIDGET_HPP
