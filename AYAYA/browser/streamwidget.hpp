#ifndef STREAMWIDGET_HPP
#define STREAMWIDGET_HPP

#include "browseritemwidget.hpp"

namespace Twitch {
class Stream;
}

class StreamWidget : public BrowserItemWidget {
    Q_OBJECT
public:
    explicit StreamWidget(const Twitch::Stream&);
    StreamWidget(const Twitch::Stream&, QWidget*);

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
