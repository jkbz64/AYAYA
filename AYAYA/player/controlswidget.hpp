#ifndef CONTROLSWIDGET_HPP
#define CONTROLSWIDGET_HPP

#include <QWidget>

class PlayerWidget;

class QPushButton;
class QSlider;
class QProgressBar;
class QPropertyAnimation;

namespace Ui {
class ControlsWidget;
}

class ControlsWidget : public QWidget {
    Q_OBJECT

public:
    explicit ControlsWidget(PlayerWidget*);
    ~ControlsWidget();

    void startFadeTimer();
    void resetFadeTimer();

    int currentVolume();

    QPushButton* restartButton() const;
    QPushButton* muteButton() const;
    QProgressBar* bufferingBar() const;
    QSlider* volumeSlider() const;
    QPushButton* theaterButton() const;
    QPushButton* fullscreenButton() const;

    void makeVisible();

signals:
    void pressedTheaterButton();
    void pressedFullscreenButton();
    void pressedRestartButton();
    void pressedMuteButton();
    void changedVolume(int);

private:
    Ui::ControlsWidget* m_ui;
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
