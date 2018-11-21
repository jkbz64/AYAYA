#include "playerwidget.hpp"
#include "controlswidget.hpp"
#include <QGridLayout>
#include <QSpacerItem>

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
    , m_currentStreamPath()
    , m_controlsWidget(nullptr)
    , m_beforeMuteVolume(65)
{
    setWindowFlags(Qt::Widget | Qt::CustomizeWindowHint);
    setBaseSize(450, 1);
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
    return m_currentStreamPath;
}

#include <QFutureWatcher>
#include <QUrl>

void PlayerWidget::openStream(const QString& streamName)
{
    if (m_streamExtractor) {
        m_currentStreamPath = streamName;
        // Fetch best quality and play it
        auto bestUrlReply = m_streamExtractor->getStreamUrl("twitch.tv/" + streamName);
        connect(bestUrlReply, &StreamUrlReply::finished, [this, bestUrlReply]() {
            if (m_impl)
                m_impl->load(bestUrlReply->result().toString());
        });
        // Fetch all qualities and fill the controls combo with them
        auto availableFormatsReply = m_streamExtractor->getStreamFormats("twitch.tv/" + streamName);
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
        m_impl->load(" ");
}

void PlayerWidget::reset()
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

void PlayerWidget::setupOverlay()
{
    auto overlayLayout = new QGridLayout(centralWidget());
    m_controlsWidget = new ControlsWidget(this);
    m_beforeMuteVolume = m_controlsWidget->currentVolume();

    connect(controlsWidget(), &ControlsWidget::pressedRestartButton, this, &PlayerWidget::reset);
    connect(controlsWidget(), &ControlsWidget::pressedMuteButton, this, &PlayerWidget::onPressedMuteButton);
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
    if (!m_currentStreamPath.isEmpty())
        openStream(m_currentStreamPath);
}

// Slots
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
        auto formatUrlReply = m_streamExtractor->getStreamUrl("twitch.tv/" + m_currentStreamPath, format);
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
