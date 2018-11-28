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
    connect(m_ui->m_stopButton, &QPushButton::pressed, this, &ControlsWidget::pressedStopButton);
    connect(m_ui->m_restartButton, &QPushButton::pressed, this, &ControlsWidget::pressedRestartButton);
    connect(m_ui->m_volumeSlider, &QSlider::valueChanged, this, &ControlsWidget::changedVolume);
    connect(m_ui->m_theaterModeButton, &QPushButton::pressed, this, &ControlsWidget::pressedTheaterButton);
    connect(m_ui->m_fullscreenButton, &QPushButton::pressed, this, &ControlsWidget::pressedFullscreenButton);
    connect(m_ui->m_formatsComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::highlighted), this, &ControlsWidget::onFormatComboActivated);
    connect(m_ui->m_formatsComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), this, &ControlsWidget::onFormatComboTextChanged);

    connect(m_ui->m_muteButton, &QPushButton::pressed, this, &ControlsWidget::onMute);

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

void ControlsWidget::makeVisible()
{
    if (!isVisible()) {
        show();
        resetFadeTimer();
    }
}

void ControlsWidget::onFadeOutTimeout()
{
    if (!m_isFormatsComboActivated)
        hide();
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
    return m_ui->m_volumeSlider->value();
}

void ControlsWidget::setFormats(const StreamFormats& formats)
{
    m_ui->m_formatsComboBox->clear();
    m_ui->m_formatsComboBox->blockSignals(true);
    m_ui->m_formatsComboBox->addItems(QStringList::fromVector(formats));
    m_ui->m_formatsComboBox->setCurrentIndex(formats.size() - 1);
    m_ui->m_formatsComboBox->blockSignals(false);
}

void ControlsWidget::onStartedLoading()
{
    m_ui->m_restartButton->blockSignals(true);
    m_loadingGif->start();
}

void ControlsWidget::onLoaded()
{
    m_loadingGif->stop();
    m_ui->m_restartButton->blockSignals(false);
}

void ControlsWidget::onEnded()
{
    m_ui->m_restartButton->blockSignals(true);
}

void ControlsWidget::onBuffering(int value)
{
    makeVisible();
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

void ControlsWidget::onFormatComboActivated(int)
{
    m_isFormatsComboActivated = true;
    m_fadeOutTimer->stop();
}

void ControlsWidget::onFormatComboTextChanged(const QString& format)
{
    m_isFormatsComboActivated = false;
    startFadeTimer();
    emit streamFormatChanged(format);
}

void ControlsWidget::onMute()
{
    if (m_ui->m_volumeSlider->value() != 0) {
        m_beforeMuteVolume = m_ui->m_volumeSlider->value();
        emit changedVolume(0);
    } else {
        emit changedVolume(m_beforeMuteVolume);
        m_beforeMuteVolume = 0;
    }
}
