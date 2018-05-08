#include "playerwidget.hpp"
#include "controlswidget.hpp"

#include <QGridLayout>
#include <QSpacerItem>

#include "mpvplayerimpl.hpp"

PlayerWidget::PlayerWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , m_impl(nullptr)
    , m_controlsWidget(nullptr)
    , m_beforeMuteVolume(65)
{
    setBaseSize(1, 1);
    setMouseTracking(true);
    // Backend defaults to MPV
    setBackend(Backend::MPV);
}

PlayerWidget::~PlayerWidget()
{
}

void PlayerWidget::setBackend(PlayerWidget::Backend backend)
{
    m_backend = backend;

    if (m_impl)
        delete m_impl;
    m_impl = new detail::MpvPlayerImpl(this);
    m_impl->init();

    if (!layout())
        setupOverlay();
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
    if (m_impl)
        m_impl->setVolume(value);
}

int PlayerWidget::volume() const
{
    if (m_impl)
        return m_impl->volume();
    return 0;
}

void PlayerWidget::setFullscreen(bool fullscreen)
{
    // FIXME Well, setting window flags causes GL context reinitialization, so we have to reset stream
    if (fullscreen) {
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        resetStream();
        showFullScreen();
    } else {
        setWindowFlags(Qt::Widget);
        resetStream();
        showNormal();
    }
}

ControlsWidget* PlayerWidget::controlsWidget()
{
    return m_controlsWidget;
}

void PlayerWidget::leaveEvent(QEvent* event)
{
    QOpenGLWidget::leaveEvent(event);
}

void PlayerWidget::mouseMoveEvent(QMouseEvent* event)
{
    QOpenGLWidget::mouseMoveEvent(event);
    m_controlsWidget->show();
    m_controlsWidget->resetFadeTimer();
}

void PlayerWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    QOpenGLWidget::mouseDoubleClickEvent(event);
    if (isFullScreen())
        setFullscreen(false);
    else
        setFullscreen(true);
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
        setVolume(m_beforeMuteVolume);
        m_beforeMuteVolume = -1;
    } else {
        m_beforeMuteVolume = controlsWidget()->currentVolume();
        setVolume(0);
    }
}

void PlayerWidget::onVolumeChanged(int value)
{
    setVolume(value);
}
