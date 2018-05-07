#ifndef CONTROLSWIDGET_HPP
#define CONTROLSWIDGET_HPP

#include <QWidget>

namespace Ui {
class ControlsWidget;
}

class PlayerWidget;

class QPropertyAnimation;

class ControlsWidget : public QWidget {
    Q_OBJECT

public:
    explicit ControlsWidget(PlayerWidget*);
    ~ControlsWidget();

    void startFadeTimer();
    void resetFadeTimer();

    int currentVolume();
signals:
    void pressedTheaterButton();
    void pressedFullscreenButton();
    void pressedRestartButton();
    void pressedMuteButton();
    void changedVolume(int);

private:
    Ui::ControlsWidget* m_ui;
    int m_beforeMuteVolume;
    QTimer* m_fadeOutTimer;
    QMovie* m_loadingGif;

private slots:
    void onStartedLoading();
    void onLoaded();
    void onEnded();
    void onBuffering(int);
    void onPositionChanged(double);
    void onVolumeChanged(double);
};

#endif // CONTROLSWIDGET_HPP
