#ifndef PLAYERWIDGET_HPP
#define PLAYERWIDGET_HPP

#include <QOpenGLWidget>

namespace detail {
class PlayerImpl;
}

class ControlsWidget;

enum class PlayerBackend {
    MPV,
    // TODO QtAV
    // TODO ??
};

enum class PlayerStyle {
    Normal,
    Theater,
    Fullscreen
};

class PlayerWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    explicit PlayerWidget(QWidget* parent = nullptr);
    ~PlayerWidget();

    void setBackend(PlayerBackend);
    const PlayerBackend& backend() const;

    void openStream(const QString&);
    void resetStream();
    void mute(bool);
    void setVolume(int);
    int volume() const;

    void setPlayerStyle(PlayerStyle style);
    const PlayerStyle& playerStyle() const;

    ControlsWidget* controlsWidget();

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

    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int, int) override;

private:
    detail::PlayerImpl* m_impl;
    PlayerBackend m_backend;
    PlayerStyle m_playerStyle;

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
};

#endif // PLAYERWIDGET_HPP
