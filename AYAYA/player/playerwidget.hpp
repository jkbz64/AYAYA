#ifndef PLAYERWIDGET_HPP
#define PLAYERWIDGET_HPP

#include <QOpenGLWidget>

namespace detail {
class PlayerImpl;
}

class ControlsWidget;

class PlayerWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    enum class Backend {
        MPV,
        // TODO QtAV
        // TODO ??
    };

    explicit PlayerWidget(QWidget* parent = nullptr);
    ~PlayerWidget();

    void setBackend(Backend);

    void openStream(const QString&);
    void resetStream();
    void mute(bool);
    void setVolume(int);
    int volume() const;

    void setFullscreen(bool);

    ControlsWidget* controlsWidget();

signals:
    void startedLoading();
    void loaded();
    void ended();
    void buffering(int);
    void resized();
    void positionChanged(double);
    void volumeChanged(int);

protected:
    virtual void leaveEvent(QEvent*) override;
    virtual void mouseMoveEvent(QMouseEvent*) override;
    virtual void mouseDoubleClickEvent(QMouseEvent*) override;

    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int, int) override;

private:
    detail::PlayerImpl* m_impl;
    Backend m_backend;

    // ControlsWidget
    void setupOverlay();
    ControlsWidget* m_controlsWidget;
    int m_beforeMuteVolume;
    // Controls slots
    void onPressedResetButton();
    void onPressedMuteButton();
    void onVolumeChanged(int);
};

#endif // PLAYERWIDGET_HPP
