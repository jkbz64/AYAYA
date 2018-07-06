#include "vlcplayerimpl.hpp"
#include "playerwidget.hpp"
#include <vlc/vlc.h>

#define qtu(i) ((i).toUtf8().constData())

#include <QMutex>
#include <QPainter>

namespace {
QMutex m_vlcMutex;

static void* videoLockCallBack(void* object, void** planes)
{
    auto self = (detail::VlcWidget*)object;
    auto& pixelBuffer = self->pixelBuffer();
    m_vlcMutex.lock();
    planes[0] = pixelBuffer.data();
    return NULL;
}

static void videoUnlockCallback(void* object, void* picture, void* const* planes)
{
    auto self = (detail::VlcWidget*)object;
    auto& pixelBuffer = self->pixelBuffer();
    pixelBuffer.setUpdate(true);
    self->update();
    m_vlcMutex.unlock();
}

static void videoDisplayCallback(void* object, void* picture) {}
}

namespace detail {

VlcPlayerImpl::VlcPlayerImpl(PlayerWidget* player)
    : PlayerImpl(player)
    , m_vlcInstance(nullptr)
    , m_vlcPlayer(nullptr)
{
    m_renderWidget = new VlcWidget(this);
}

VlcPlayerImpl::~VlcPlayerImpl()
{
    if (m_vlcInstance)
        libvlc_release(m_vlcInstance);
}

bool VlcPlayerImpl::init()
{
    const char* const vlc_args[] = {
        "-I=core,http",
        "--no-xlib",
        "--no-video-title-show"
    };

    m_vlcInstance = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
    if (!m_vlcInstance)
        return false;

    return true;
}

void VlcPlayerImpl::load(const QString& path)
{
    if (m_vlcPlayer) { // Stop and delete player
        libvlc_media_player_stop(m_vlcPlayer);
        libvlc_media_player_release(m_vlcPlayer);
    }

    libvlc_media_t* vlcMedia = libvlc_media_new_location(m_vlcInstance, qtu(path));
    if (!vlcMedia)
        throw std::runtime_error("Failed to open path");

    m_vlcPlayer = libvlc_media_player_new_from_media(vlcMedia);
    if (!m_vlcPlayer)
        throw std::runtime_error("Failed to create VLC player from media");

    // Set callbacks for drawing to pixelbuffer
    libvlc_video_set_callbacks(m_vlcPlayer, videoLockCallBack, videoUnlockCallback, videoDisplayCallback, m_renderWidget);
    libvlc_video_set_format(m_vlcPlayer, "RV24", 1920, 1080, 1920 * 3);

    // Start playing immediately
    libvlc_media_player_play(m_vlcPlayer);
}

QString VlcPlayerImpl::currentPath()
{
    // TODO libvlc_media_get_
    return QString();
}

void VlcPlayerImpl::setVolume(int vol)
{
    libvlc_audio_set_volume(m_vlcPlayer, vol);
}

int VlcPlayerImpl::volume() const
{
    return libvlc_audio_get_volume(m_vlcPlayer);
}

VlcWidget::VlcWidget(VlcPlayerImpl* impl)
    : m_impl(impl)
    , m_buffer(QSize(1920, 1080))
{
}

VlcWidget::~VlcWidget()
{
}

PixelBuffer& VlcWidget::pixelBuffer()
{
    return m_buffer;
}

void VlcWidget::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    if (m_vlcMutex.tryLock()) {
        if (pixelBuffer().shouldUpdate()) {
            m_image = QImage(pixelBuffer().data(), 1920, 1080, QImage::Format_RGB888);
            pixelBuffer().setUpdate(false);
        }
        m_vlcMutex.unlock();
    }
    painter.drawImage(0, 0, m_image.scaled(m_impl->player()->width(), m_impl->player()->height()));
}

// Pixel buffer
PixelBuffer::PixelBuffer(const QSize& size, int bpp)
{
    m_data = new unsigned char[size.width() * size.height() * bpp];
    m_shouldUpdate = false;
}

PixelBuffer::~PixelBuffer()
{
    m_vlcMutex.lock();
    delete m_data;
    m_vlcMutex.unlock();
}

bool PixelBuffer::shouldUpdate() const
{
    return m_shouldUpdate;
}

void PixelBuffer::setUpdate(bool should)
{
    m_shouldUpdate = should;
}

unsigned char* PixelBuffer::data() const
{
    return m_data;
}
}
