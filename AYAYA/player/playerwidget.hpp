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

    ControlsWidget* controlsWidget();

signals:
    void startedLoading();
    void loaded();
    void ended();
    void buffering(int);
    void resized();
    void positionChanged(double);
    void volumeChanged(double);

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int, int) override;

private:
    detail::PlayerImpl* m_impl;
    Backend m_backend;

    ControlsWidget* m_controlsWidget;
    void setupOverlay();
};

#endif // PLAYERWIDGET_HPP
