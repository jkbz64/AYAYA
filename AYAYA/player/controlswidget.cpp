#include "controlswidget.hpp"
#include "playerwidget.hpp"
#include "ui_controlswidget.h"

#include <QDebug>
#include <QMovie>
#include <QTimer>

#include <QPropertyAnimation>

ControlsWidget::ControlsWidget(PlayerWidget* player)
    : QWidget(player)
    , m_ui(new Ui::ControlsWidget)
    , m_fadeOutTimer(new QTimer(this))
    , m_beforeMuteVolume(-1)
    , m_loadingGif(new QMovie(QString(":/gifs/clap.gif"), QByteArray(), this))
{
    m_ui->setupUi(this);
    m_ui->m_bufferingBar->hide();

    m_fadeOutTimer->setSingleShot(true);
    connect(m_fadeOutTimer, &QTimer::timeout, this, &ControlsWidget::hide);

    connect(player, &PlayerWidget::positionChanged, this, &ControlsWidget::onPositionChanged);
    connect(player, &PlayerWidget::volumeChanged, this, &ControlsWidget::onVolumeChanged);
    connect(player, &PlayerWidget::buffering, this, &ControlsWidget::onBuffering);
    connect(player, &PlayerWidget::startedLoading, this, &ControlsWidget::onStartedLoading);
    connect(player, &PlayerWidget::loaded, this, &ControlsWidget::onLoaded);
    connect(player, &PlayerWidget::ended, this, &ControlsWidget::onEnded);
    connect(m_ui->m_restartButton, &QPushButton::pressed, [this, player]() {
        if (!m_ui->m_restartButton->signalsBlocked())
            player->resetStream();
    });
    connect(m_loadingGif, &QMovie::frameChanged, [this]() {
        m_ui->m_restartButton->setIconSize(QSize(60, 60));
        m_ui->m_restartButton->setIcon(QIcon(m_loadingGif->currentPixmap()));
    });
    connect(m_ui->m_muteButton, &QPushButton::released, [this, player]() {
        if (m_beforeMuteVolume != -1) {
            player->setVolume(m_beforeMuteVolume);
            m_beforeMuteVolume = -1;
        } else {
            m_beforeMuteVolume = m_ui->m_volumeSlider->value();
            player->setVolume(0);
        }
    });
    connect(m_ui->m_volumeSlider, &QSlider::valueChanged, player, &PlayerWidget::setVolume);
    connect(m_ui->m_fullscreenButton, &QPushButton::released, [this, player]() {
        if (player->isFullScreen())
            player->setFullscreen(false);
        else
            player->setFullscreen(true);
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

void ControlsWidget::onStartedLoading()
{
    m_ui->m_restartButton->blockSignals(true);
    m_ui->m_restartButton->setText("");
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
