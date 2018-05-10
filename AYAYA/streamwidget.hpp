#ifndef STREAMWIDGET_HPP
#define STREAMWIDGET_HPP

#include "mainwidget.hpp"
#include "player/playerwidget.hpp"

namespace Twitch {
class Stream;
class User;
}

class ChatWidget;

namespace Ui {
class StreamWidget;
}

class StreamWidget : public MainWidget {
    Q_OBJECT

public:
    explicit StreamWidget(QWidget* parent = 0);
    ~StreamWidget();
    virtual void init() override;

    void initialize(const Twitch::User&, const Twitch::Stream&);

    PlayerWidget* player() const;
    ChatWidget* chat() const;

signals:
    void enteredTheaterMode();
    void enteredFullscreenMode();
    void leftWindowMode();

private:
    Ui::StreamWidget* m_ui;

    void onSplitterMoved();
    void onPlayerStyleChanged(PlayerStyle, PlayerStyle);
    void onChatProgress();
};

#endif // LIVESTREAMWIDGET_HPP
