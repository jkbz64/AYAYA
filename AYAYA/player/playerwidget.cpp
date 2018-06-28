#include "playerwidget.hpp"
#include "controlswidget.hpp"
#include <QGridLayout>
#include <QSpacerItem>

#include "nullplayerimpl.hpp"

#ifdef MPV
#include "mpvplayerimpl.hpp"
#endif

#include <QLabel>

PlayerWidget::PlayerWidget(QWidget* parent)
    : QMainWindow(parent)
    , m_impl(nullptr)
    , m_playerStyle(PlayerStyle::Normal)
    , m_controlsWidget(nullptr)
    , m_beforeMuteVolume(65)
{
    setWindowFlags(Qt::Widget | Qt::CustomizeWindowHint);
    setBaseSize(450, 1);
    setMouseTracking(true);
}

PlayerWidget::~PlayerWidget()
{
    delete m_impl;
}

ControlsWidget* PlayerWidget::controlsWidget()
{
    return m_controlsWidget;
}

void PlayerWidget::setBackend(PlayerBackend backend)
{
    m_backend = backend;
    if (m_impl)
        delete m_impl;

    switch (backend) {
    case PlayerBackend::Null:
        m_impl = new detail::NullPlayerImpl(this);
        break;
    case PlayerBackend::Mpv:
#ifdef MPV
        m_impl = new detail::MpvPlayerImpl(this);
#else
        throw std::runtime_error("AYAYA was not built with Mpv, compile AYAYA with CONFIG+=\"MPV\"");
#endif
        break;
    }

    emit startedBackendInit();
    if (m_impl->init()) {
        const auto render = m_impl->renderWidget();
        render->setMouseTracking(true);
        setCentralWidget(render);
    }

    if (backend != PlayerBackend::Null)
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

void PlayerWidget::mouseMoveEvent(QMouseEvent* event)
{
    QMainWindow::mouseMoveEvent(event);
    if (backend() != PlayerBackend::Null)
        controlsWidget()->makeVisible();
}

void PlayerWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    QMainWindow::mouseDoubleClickEvent(event);
    if (playerStyle() == PlayerStyle::Fullscreen || playerStyle() == PlayerStyle::Theater)
        setPlayerStyle(PlayerStyle::Normal);
    else
        setPlayerStyle(PlayerStyle::Theater);
}

void PlayerWidget::setupOverlay()
{
    auto overlayLayout = new QGridLayout(centralWidget());
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
    centralWidget()->setLayout(overlayLayout);
}

// Slots
void PlayerWidget::onPressedResetButton()
{
    resetStream();
}

void PlayerWidget::onPressedMuteButton()
{
    if (volume() != 0) {
        m_beforeMuteVolume = volume();
        setVolume(0);
    } else {
        setVolume(m_beforeMuteVolume);
        m_beforeMuteVolume = 0;
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

#include <QEvent>

bool PlayerWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_impl->renderWidget()) {
        if (event->type() == QEvent::MouseMove) {
            controlsWidget()->makeVisible();
        }
    } else {
        return QMainWindow::eventFilter(watched, event);
    }
}
