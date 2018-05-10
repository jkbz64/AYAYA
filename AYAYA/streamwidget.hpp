#ifndef STREAMWIDGET_HPP
#define STREAMWIDGET_HPP

#include "chat/emotescache.hpp"
#include "initwidget.hpp"
#include "player/playerwidget.hpp"

namespace Twitch {
class Stream;
class User;
}

class ChatWidget;

namespace Ui {
class StreamWidget;
}

class StreamWidget : public InitWidget {
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

protected:
    virtual bool checkInitStatus() override;

private:
    Ui::StreamWidget* m_ui;

    // Init slots
    void onStartedBackendInit();
    void onBackendChanged(PlayerBackend);
    void onStartedInitingCache();
    void onCacheInitProgress(int);
    void onEndedInitingCache();

    void onStartedFetchingGlobalEmotes();
    void onGlobalEmotesFetchProgress(EmotesBackend, const QString&, const QString&);
    void onFetchedGlobalEmotes();

    void onSplitterMoved();
    void onPlayerStyleChanged(PlayerStyle, PlayerStyle);
    void onChatProgress();
};

#endif // LIVESTREAMWIDGET_HPP
