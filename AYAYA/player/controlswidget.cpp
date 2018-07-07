#include "controlswidget.hpp"
#include "playerwidget.hpp"
#include "ui_controlswidget.h"
#include <QMovie>
#include <QPropertyAnimation>
#include <QTimer>

ControlsWidget::ControlsWidget(PlayerWidget* player)
    : QWidget(player)
    , m_ui(new Ui::ControlsWidget)
    , m_fadeOutTimer(new QTimer(this))
    , m_loadingGif(new QMovie(QString(":/gifs/clap.gif"), QByteArray(), this))
{
    m_ui->setupUi(this);
    // Pre-setup
    m_ui->m_bufferingBar->hide();

    // Timer
    m_fadeOutTimer->setSingleShot(true);
    connect(m_fadeOutTimer, &QTimer::timeout, this, &ControlsWidget::hide);

    // PlayerWidget signals handling
    connect(player, &PlayerWidget::positionChanged, this, &ControlsWidget::onPositionChanged);
    connect(player, &PlayerWidget::volumeChanged, this, &ControlsWidget::onVolumeChanged);
    connect(player, &PlayerWidget::buffering, this, &ControlsWidget::onBuffering);
    connect(player, &PlayerWidget::startedLoading, this, &ControlsWidget::onStartedLoading);
    connect(player, &PlayerWidget::loaded, this, &ControlsWidget::onLoaded);
    connect(player, &PlayerWidget::ended, this, &ControlsWidget::onEnded);

    // Controls Widgets
    connect(restartButton(), &QPushButton::pressed, this, &ControlsWidget::pressedRestartButton);
    connect(muteButton(), &QPushButton::pressed, this, &ControlsWidget::pressedMuteButton);
    connect(volumeSlider(), &QSlider::valueChanged, this, &ControlsWidget::changedVolume);
    connect(theaterButton(), &QPushButton::pressed, this, &ControlsWidget::pressedTheaterButton);
    connect(fullscreenButton(), &QPushButton::pressed, this, &ControlsWidget::pressedFullscreenButton);

    // Gif
    connect(m_loadingGif, &QMovie::frameChanged, [this]() {
        m_ui->m_restartButton->setIconSize(QSize(60, 60));
        m_ui->m_restartButton->setIcon(QIcon(m_loadingGif->currentPixmap()));
    });
}

ControlsWidget::~ControlsWidget()
{
    delete m_ui;
}

QPushButton* ControlsWidget::restartButton() const
{
    return m_ui->m_restartButton;
}

QPushButton* ControlsWidget::muteButton() const
{
    return m_ui->m_muteButton;
}

QSlider* ControlsWidget::volumeSlider() const
{
    return m_ui->m_volumeSlider;
}

QProgressBar* ControlsWidget::bufferingBar() const
{
    return m_ui->m_bufferingBar;
}

QPushButton* ControlsWidget::theaterButton() const
{
    return m_ui->m_theaterModeButton;
}

QPushButton* ControlsWidget::fullscreenButton() const
{
    return m_ui->m_fullscreenButton;
}

void ControlsWidget::makeVisible()
{
    show();
    resetFadeTimer();
}

void ControlsWidget::startFadeTimer()
{
    m_fadeOutTimer->start();
    m_fadeOutTimer->setInterval(500);
}

void ControlsWidget::resetFadeTimer()
{
    m_fadeOutTimer->setInterval(500);
    m_fadeOutTimer->start();
}

int ControlsWidget::currentVolume()
{
    return volumeSlider()->value();
}

void ControlsWidget::onStartedLoading()
{
    restartButton()->blockSignals(true);
    m_loadingGif->start();
}

void ControlsWidget::onLoaded()
{
    m_loadingGif->stop();
    restartButton()->setText("Restart");
    restartButton()->blockSignals(false);
}

void ControlsWidget::onEnded()
{
    restartButton()->blockSignals(true);
}

void ControlsWidget::onBuffering(int value)
{
    bufferingBar()->show();
    bufferingBar()->setValue(value);
    makeVisible();
}

void ControlsWidget::onPositionChanged(double)
{
    bufferingBar()->hide();
}

void ControlsWidget::onVolumeChanged(double value)
{
    volumeSlider()->blockSignals(true);
    volumeSlider()->setValue(static_cast<int>(value));
    volumeSlider()->blockSignals(false);
}
