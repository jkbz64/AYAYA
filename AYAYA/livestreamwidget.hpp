#ifndef LIVESTREAMWIDGET_HPP
#define LIVESTREAMWIDGET_HPP

#include <QWidget>

namespace Twitch {
class Stream;
class User;
}

class PlayerWidget;

namespace Ui {
class LivestreamWidget;
}

class LivestreamWidget : public QWidget {
    Q_OBJECT

public:
    explicit LivestreamWidget(QWidget* parent = 0);
    ~LivestreamWidget();

    void initialize(const Twitch::User&, const Twitch::Stream&);

private:
    Ui::LivestreamWidget* m_ui;
};

#endif // LIVESTREAMWIDGET_HPP
