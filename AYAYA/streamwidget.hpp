#ifndef STREAMWIDGET_HPP
#define STREAMWIDGET_HPP

#include <QWidget>

namespace Twitch {
class Stream;
class User;
}

class PlayerWidget;

namespace Ui {
class StreamWidget;
}

class StreamWidget : public QWidget {
    Q_OBJECT

public:
    explicit StreamWidget(QWidget* parent = 0);
    ~StreamWidget();

    void initialize(const Twitch::User&, const Twitch::Stream&);

signals:
    void enteredTheaterMode();
    void leftTheaterMode();
    void enteredFullscreenMode();
    void leftFullscreenMode();

private:
    Ui::StreamWidget* m_ui;

    enum class WatchMode {
        Normal,
        Theater,
        Fullscreen
    } m_currentMode;

    void onSplitterMoved();
    void onPressedTheaterButton();
    void onPressedFullscreenButton();
};

#endif // LIVESTREAMWIDGET_HPP
