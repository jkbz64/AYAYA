#include "playerwidget.hpp"
#include "controlswidget.hpp"

#include <QGridLayout>
#include <QSpacerItem>

#include "mpvplayerimpl.hpp"

PlayerWidget::PlayerWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , m_impl(nullptr)
    , m_playerStyle(PlayerStyle::Normal)
    , m_controlsWidget(nullptr)
    , m_beforeMuteVolume(65)
{
    setBaseSize(1, 1);
    setMouseTracking(true);
}

PlayerWidget::~PlayerWidget()
{
    delete m_impl;
}

void PlayerWidget::setBackend(PlayerBackend backend)
{
    m_backend = backend;

    if (m_impl)
        delete m_impl;

    m_impl = new detail::MpvPlayerImpl(this);

    emit startedBackendInit();
    m_impl->init();

    // Install overlay when setting backend first time
    if (!layout())
        setupOverlay();

    emit backendChanged(backend);
}

const PlayerBackend& PlayerWidget::backend() const
{
    return m_backend;
}

void PlayerWidget::openStream(const QString& streamName)
{
    if (m_impl)
        m_impl->load(QString("https://www.twitch.tv/" + streamName));
}

void PlayerWidget::resetStream()
{
    if (m_impl)
        m_impl->load(m_impl->currentPath());
}

void PlayerWidget::setVolume(int value)
{
    if (value > 0)
        m_beforeMuteVolume = value;

    if (m_impl)
        m_impl->setVolume(value);
}

int PlayerWidget::volume() const
{
    if (m_impl)
        return m_impl->volume();
    return 0;
}

void PlayerWidget::setPlayerStyle(PlayerStyle style)
{
    if (m_playerStyle != style) {
        const auto oldStyle = m_playerStyle;
        m_playerStyle = style;
        emit playerStyleChanged(oldStyle, m_playerStyle);
    }
}

const PlayerStyle& PlayerWidget::playerStyle() const
{
    return m_playerStyle;
}

ControlsWidget* PlayerWidget::controlsWidget()
{
    return m_controlsWidget;
}

void PlayerWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_controlsWidget->isHidden())
        m_controlsWidget->show();
    m_controlsWidget->resetFadeTimer();
    QOpenGLWidget::mouseMoveEvent(event);
}

void PlayerWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    QOpenGLWidget::mouseDoubleClickEvent(event);
    if (playerStyle() == PlayerStyle::Fullscreen || playerStyle() == PlayerStyle::Theater)
        setPlayerStyle(PlayerStyle::Normal);
    else
        setPlayerStyle(PlayerStyle::Theater);
}

void PlayerWidget::initializeGL()
{
    QOpenGLWidget::initializeGL();
    if (m_impl)
        m_impl->initializeGL();
}

void PlayerWidget::paintGL()
{
    QOpenGLWidget::paintGL();
    if (m_impl)
        m_impl->paintGL();
}

void PlayerWidget::resizeGL(int w, int h)
{
    if (m_impl && !m_impl->resizeGL(w, h))
        QOpenGLWidget::resizeGL(w, h);
}

void PlayerWidget::setupOverlay()
{
    auto overlayLayout = new QGridLayout();
    m_controlsWidget = new ControlsWidget(this);
    m_beforeMuteVolume = m_controlsWidget->currentVolume();

    connect(m_controlsWidget, &ControlsWidget::pressedRestartButton, this, &PlayerWidget::onPressedResetButton);
    connect(m_controlsWidget, &ControlsWidget::pressedMuteButton, this, &PlayerWidget::onPressedMuteButton);
    connect(m_controlsWidget, &ControlsWidget::changedVolume, this, &PlayerWidget::onVolumeChanged);
    connect(m_controlsWidget, &ControlsWidget::pressedTheaterButton, this, &PlayerWidget::onPressedTheaterButton);
    connect(m_controlsWidget, &ControlsWidget::pressedFullscreenButton, this, &PlayerWidget::onPressedFullscreenButton);

    m_controlsWidget->startFadeTimer();

    overlayLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding), 0, 0, 12, 1);
    overlayLayout->addWidget(m_controlsWidget, 12, 0, 2, 1);

    setLayout(overlayLayout);
}

void PlayerWidget::onPressedResetButton()
{
    resetStream();
}

void PlayerWidget::onPressedMuteButton()
{
    if (m_beforeMuteVolume != -1) {
        setVolume(0);
        m_beforeMuteVolume = -1;
    } else {
        setVolume(m_beforeMuteVolume);
    }
}

void PlayerWidget::onVolumeChanged(int value)
{
    setVolume(value);
}

void PlayerWidget::onPressedTheaterButton()
{
    if (m_playerStyle != PlayerStyle::Theater)
        setPlayerStyle(PlayerStyle::Theater);
    else
        setPlayerStyle(PlayerStyle::Normal);
}

void PlayerWidget::onPressedFullscreenButton()
{
    if (m_playerStyle != PlayerStyle::Fullscreen)
        setPlayerStyle(PlayerStyle::Fullscreen);
    else
        setPlayerStyle(PlayerStyle::Normal);
}
