#ifndef CONTROLSWIDGET_HPP
#define CONTROLSWIDGET_HPP

#include <QWidget>

class PlayerWidget;

class QPushButton;
class QSlider;
class QProgressBar;
class QPropertyAnimation;
class QComboBox;

using StreamFormat = QString;
using StreamFormats = QVector<StreamFormat>;

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

    void setFormats(const StreamFormats&);

    QPushButton* restartButton() const;
    QPushButton* muteButton() const;
    QProgressBar* bufferingBar() const;
    QSlider* volumeSlider() const;
    QPushButton* theaterButton() const;
    QPushButton* fullscreenButton() const;
    QComboBox* formatsComboBox() const;

    void makeVisible();

signals:
    void pressedTheaterButton();
    void pressedFullscreenButton();
    void pressedRestartButton();
    void pressedMuteButton();
    void changedVolume(int);
    void streamFormatChanged(const StreamFormat&);

private:
    Ui::ControlsWidget* m_ui;
    QTimer* m_fadeOutTimer;
    QMovie* m_loadingGif;
    bool m_isFormatsComboActivated;

private slots:
    void onFadeOutTimeout();
    void onStartedLoading();
    void onLoaded();
    void onEnded();
    void onBuffering(int);
    void onPositionChanged(double);
    void onVolumeChanged(double);
    void onFormatComboActivated(int);
    void onFormatComboTextChanged(const QString&);
};

#endif // CONTROLSWIDGET_HPP
