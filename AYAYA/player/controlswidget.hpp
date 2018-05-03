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

private:
    Ui::ControlsWidget* m_ui;
    QTimer* m_fadeOutTimer;
    int m_beforeMuteVolume;
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
