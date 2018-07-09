#ifndef PLAYERWIDGET_HPP
#define PLAYERWIDGET_HPP

#include <QMainWindow>

namespace detail {
class PlayerImpl;
}

class StreamExtractor;
class ControlsWidget;

using StreamFormat = QString;

enum class PlayerBackend {
    Null,
    Mpv,
    Vlc
    // TODO ??
};

enum class ExtractorBackend {
    Null,
    Ytdl,
    //Streamlink
};

enum class PlayerStyle {
    Normal,
    Theater,
    Fullscreen
};

class PlayerWidget : public QMainWindow {
    Q_OBJECT
public:
    explicit PlayerWidget(QWidget* parent = nullptr);
    ~PlayerWidget();

    void setBackend(PlayerBackend);
    const PlayerBackend& backend() const;

    void setExtractor(ExtractorBackend);
    const ExtractorBackend& extractorBackend() const;

    void openStream(const QString&);
    void resetStream();
    void mute(bool);
    void setVolume(int);
    int volume() const;

    void setPlayerStyle(PlayerStyle style);
    const PlayerStyle& playerStyle() const;

    ControlsWidget* controlsWidget();

    virtual bool eventFilter(QObject*, QEvent*) override;

signals:
    // Backend
    void startedBackendInit();
    void backendChanged(PlayerBackend);

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
    PlayerBackend m_backend;
    StreamExtractor* m_streamExtractor;
    ExtractorBackend m_extractorBackend;
    PlayerStyle m_playerStyle;

    QString m_currentStreamName;

    // ControlsWidget
    void setupOverlay();
    ControlsWidget* m_controlsWidget;
    int m_beforeMuteVolume;

private slots:
    // Controls slots
    void onPressedResetButton();
    void onPressedMuteButton();
    void onVolumeChanged(int);
    void onPressedTheaterButton();
    void onPressedFullscreenButton();
    void onStreamFormatChanged(const StreamFormat&);
};

#endif // PLAYERWIDGET_HPP
