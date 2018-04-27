#ifndef STREAMBROWSER_HPP
#define STREAMBROWSER_HPP

#include <QObject>
#include <QScrollArea>

namespace Twitch {
class Stream;
}

class StreamWidget;

class StreamBrowser : public QScrollArea {
    Q_OBJECT
public:
    explicit StreamBrowser(QWidget* = nullptr);
    StreamWidget* addStream(const Twitch::Stream&);
    void clear();
signals:
    void streamSelected(const Twitch::Stream&);
    void updateRequested(StreamWidget*);

private:
    QVector<StreamWidget*> m_visibleWidgets;
};

#endif // STREAMBROWSER_HPP
