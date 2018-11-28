#include "playerwidget.hpp"
#include "controlswidget.hpp"
#include <QGridLayout>
#include <QSpacerItem>

#include <TwitchQt/twitchstream.hpp>

#include "backends/nullplayerimpl.hpp"

#ifdef MPV
#include "backends/mpvplayerimpl.hpp"
#endif

#ifdef VLC
#include "backends/vlcplayerimpl.hpp"
#endif

#include "extractors/ytdlextractor.hpp"

#include <QMessageBox>

PlayerWidget::PlayerWidget(QWidget* parent)
    : QMainWindow(parent)
    , m_impl(nullptr)
    , m_streamExtractor(nullptr)
    , m_playerStyle(PlayerStyle::Normal)
    , m_controlsWidget(nullptr)
{
    setWindowFlags(Qt::Widget | Qt::CustomizeWindowHint);
    setMinimumWidth(365);
    setMouseTracking(true);

    connect(this, &PlayerWidget::backendChanged, this, &PlayerWidget::onPlayerBackendChanged);
}

PlayerWidget::~PlayerWidget()
{
    delete m_impl;
}

ControlsWidget* PlayerWidget::controlsWidget()
{
    return m_controlsWidget;
}

void PlayerWidget::setBackend(Backend backend)
{
    m_backend = backend;
    if (m_impl)
        delete m_impl;

    switch (backend) {
    case Backend::Null:
        m_impl = new detail::NullPlayerImpl(this);
        break;
    case Backend::Mpv:
#ifdef MPV
        m_impl = new detail::MpvPlayerImpl(this);
#else
        QMessageBox::warning(this, "Unsupported Backend", "Selected backend was not compiled into AYAYA, fallbacking to Null Backend", QMessageBox::Ok);
        m_impl = new detail::NullPlayerImpl(this);
#endif
        break;
    case Backend::Vlc:
#ifdef VLC
        m_impl = new detail::VlcPlayerImpl(this);
#else
        QMessageBox::warning(this, "Unsupported Backend", "Selected backend was not compiled into AYAYA, fallbacking to Null Backend", QMessageBox::Ok);
        m_impl = new detail::NullPlayerImpl(this);
#endif
        break;
    }

    emit startedBackendInit();
    if (m_impl->init()) {
        const auto render = m_impl->renderWidget();
        setCentralWidget(render);
        render->setMouseTracking(true);
    }

    if (backend != Backend::Null)
        setupOverlay();

    emit backendChanged(backend);
}

const PlayerWidget::Backend& PlayerWidget::backend() const
{
    return m_backend;
}

void PlayerWidget::setExtractor(ExtractorBackend backend)
{
    m_extractorBackend = backend;

    switch (backend) {
    case ExtractorBackend::Null:
        m_streamExtractor = nullptr;
        break;

    case ExtractorBackend::Ytdl:
        m_streamExtractor = new YtdlExtractor(this);
        break;
    }

    emit extractorChanged(backend);
}

const PlayerWidget::ExtractorBackend& PlayerWidget::extractorBackend() const
{
    return m_extractorBackend;
}

QString PlayerWidget::streamPath() const
{
    if (m_currentStream.m_id.isEmpty())
        return QString();
    else
        return QString("https://twitch.tv/") + m_currentStream.m_userName;
}

#include <QFutureWatcher>
#include <QUrl>

void PlayerWidget::openStream(const Twitch::Stream& stream)
{
    stop();
    setVolume(defaultVolume());
    if (m_streamExtractor) {
        m_currentStream = stream;
        // Fetch best quality and play it
        auto bestUrlReply = m_streamExtractor->getStreamUrl(streamPath());
        connect(bestUrlReply, &StreamUrlReply::finished, [this, bestUrlReply]() {
            if (m_impl)
                m_impl->load(bestUrlReply->result().toString());
        });
        // Fetch all qualities and fill the controls combo with them
        auto availableFormatsReply = m_streamExtractor->getStreamFormats(streamPath());
        connect(availableFormatsReply, &StreamFormatsReply::finished, [this, availableFormatsReply]() {
            if (controlsWidget())
                controlsWidget()->setFormats(availableFormatsReply->result());
        });
    } else
        QMessageBox::warning(this, "No extractor has been set", "The extractor has not been selected", QMessageBox::Ok);
}

void PlayerWidget::stop()
{
    if (m_impl)
        m_impl->load("");
}

void PlayerWidget::reset()
{
    if (m_impl && !streamPath().isEmpty())
        openStream(m_currentStream);
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
    if (backend() != Backend::Null)
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

int PlayerWidget::defaultVolume() const
{
    return m_defaultVolume;
}

void PlayerWidget::setDefaultVolume(int defaultVolume)
{
    m_defaultVolume = defaultVolume;
}

void PlayerWidget::setupOverlay()
{
    auto overlayLayout = new QGridLayout(centralWidget());
    m_controlsWidget = new ControlsWidget(this);

    connect(controlsWidget(), &ControlsWidget::pressedStopButton, this, &PlayerWidget::stop);
    connect(controlsWidget(), &ControlsWidget::pressedRestartButton, this, &PlayerWidget::reset);
    connect(controlsWidget(), &ControlsWidget::changedVolume, this, &PlayerWidget::setVolume);
    connect(controlsWidget(), &ControlsWidget::pressedTheaterButton, this, &PlayerWidget::onPressedTheaterButton);
    connect(controlsWidget(), &ControlsWidget::pressedFullscreenButton, this, &PlayerWidget::onPressedFullscreenButton);
    connect(controlsWidget(), &ControlsWidget::streamFormatChanged, this, &PlayerWidget::onStreamFormatChanged);

    controlsWidget()->startFadeTimer();

    overlayLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding), 0, 0, 12, 1);
    overlayLayout->addWidget(m_controlsWidget, 12, 0, 2, 1);
    centralWidget()->setLayout(overlayLayout);
    centralWidget()->setMinimumWidth(controlsWidget()->minimumSizeHint().width());
}

void PlayerWidget::onPlayerBackendChanged(PlayerWidget::Backend)
{
    if (!m_currentStream.m_id.isEmpty())
        openStream(m_currentStream);
}

// Slots
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

void PlayerWidget::onStreamFormatChanged(const StreamFormat& format)
{
    if (!format.isEmpty()) { // In case no signal-blocking at the startup
        auto formatUrlReply = m_streamExtractor->getStreamUrl(streamPath(), format);
        connect(formatUrlReply, &StreamUrlReply::finished, [this, formatUrlReply]() {
            if (m_impl)
                m_impl->load(formatUrlReply->result().toString());
        });
    }
}

#include <QEvent>

bool PlayerWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_impl->renderWidget()) {
        if (event->type() == QEvent::MouseMove) {
            if (controlsWidget())
                controlsWidget()->makeVisible();
        }
    }
    return QMainWindow::eventFilter(watched, event);
}
