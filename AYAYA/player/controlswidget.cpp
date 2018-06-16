#include "controlswidget.hpp"
#include "playerwidget.hpp"
#include "ui_controlswidget.h"
#include <QMovie>
#include <QPropertyAnimation>
#include <QTimer>

ControlsWidget::ControlsWidget(PlayerWidget* player)
    : QWidget(player)
    , m_ui(new Ui::ControlsWidget)
    , m_beforeMuteVolume(-1)
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
    connect(m_ui->m_restartButton, &QPushButton::pressed, this, &ControlsWidget::pressedRestartButton);
    connect(m_ui->m_muteButton, &QPushButton::pressed, this, &ControlsWidget::pressedMuteButton);
    connect(m_ui->m_volumeSlider, &QSlider::valueChanged, this, &ControlsWidget::changedVolume);
    connect(m_ui->m_theaterModeButton, &QPushButton::pressed, this, &ControlsWidget::pressedTheaterButton);
    connect(m_ui->m_fullscreenButton, &QPushButton::pressed, this, &ControlsWidget::pressedFullscreenButton);

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
    return m_ui->m_volumeSlider->value();
}

void ControlsWidget::onStartedLoading()
{
    m_ui->m_restartButton->blockSignals(true);
    m_loadingGif->start();
}

void ControlsWidget::onLoaded()
{
    m_loadingGif->stop();
    m_ui->m_restartButton->setText("Restart");
    m_ui->m_restartButton->blockSignals(false);
}

void ControlsWidget::onEnded()
{
    m_ui->m_restartButton->blockSignals(true);
}

void ControlsWidget::onBuffering(int value)
{
    m_ui->m_bufferingBar->show();
    m_ui->m_bufferingBar->setValue(value);
}

void ControlsWidget::onPositionChanged(double)
{
    m_ui->m_bufferingBar->hide();
}

void ControlsWidget::onVolumeChanged(double value)
{
    m_ui->m_volumeSlider->blockSignals(true);
    m_ui->m_volumeSlider->setValue(static_cast<int>(value));
    m_ui->m_volumeSlider->blockSignals(false);
}
