#ifndef PLAYERWIDGET_HPP
#define PLAYERWIDGET_HPP

#include <QMainWindow>
#include <TwitchQt/twitchstream.hpp>

namespace detail {
class PlayerImpl;
}

namespace Twitch {
class Stream;
}

class StreamExtractor;
class ControlsWidget;

using StreamFormat = QString;

enum class PlayerStyle {
    Normal,
    Theater,
    Fullscreen
};

class PlayerWidget : public QMainWindow {
    Q_OBJECT
public:
    enum class Backend {
        Null,
        Mpv,
        Vlc
        // TODO ??
    };
    Q_ENUM(Backend)

    enum class ExtractorBackend {
        Null,
        Ytdl,
        Streamlink
    };
    Q_ENUM(ExtractorBackend)

    explicit PlayerWidget(QWidget* parent = nullptr);
    ~PlayerWidget();

    void setBackend(Backend);
    const Backend& backend() const;

    void setExtractor(ExtractorBackend);
    const ExtractorBackend& extractorBackend() const;

    QString streamPath() const;
    void openStream(const Twitch::Stream&);
    void stop();
    void reset();
    void mute(bool);
    void setVolume(int);
    int volume() const;

    void setPlayerStyle(PlayerStyle style);
    const PlayerStyle& playerStyle() const;

    ControlsWidget* controlsWidget();

    virtual bool eventFilter(QObject*, QEvent*) override;

    int defaultVolume() const;
    void setDefaultVolume(int);

    int hideControlsTimeout() const;
    void setHideControlsTimeout(int hideControlsTimeout);

signals:
    // Backend
    void startedBackendInit();
    void backendChanged(Backend);
    void extractorChanged(ExtractorBackend);

    // Stream
    void startedLoading();
    void loaded();
    void ended();
    void buffering(int);
    void resized();
    void positionChanged(double);
    void volumeChanged(int);

    // Player-specific
    void playerStyleChanged(PlayerStyle, PlayerStyle);

protected:
    virtual void mouseMoveEvent(QMouseEvent*) override;
    virtual void mouseDoubleClickEvent(QMouseEvent*) override;

private:
    detail::PlayerImpl* m_impl;
    Backend m_backend;
    StreamExtractor* m_streamExtractor;
    ExtractorBackend m_extractorBackend;
    PlayerStyle m_playerStyle;

    Twitch::Stream m_currentStream;

    int m_defaultVolume;
    int m_hideControlsTimeout;

    // ControlsWidget
    void setupOverlay();
    ControlsWidget* m_controlsWidget;

private slots:
    void onPlayerBackendChanged(Backend);

    // Controls slots
    void onPressedTheaterButton();
    void onPressedFullscreenButton();
    void onStreamFormatChanged(const StreamFormat&);
};

#endif // PLAYERWIDGET_HPP
