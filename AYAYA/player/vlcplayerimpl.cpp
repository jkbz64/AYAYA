#include "vlcplayerimpl.hpp"
#include <QWidget>
#include <QWindow>
#include <vlc/vlc.h>

#define qtu(i) ((i).toUtf8().constData())

namespace detail {

VlcPlayerImpl::VlcPlayerImpl(PlayerWidget* player)
    : PlayerImpl(player)
    , m_vlcInstance(nullptr)
    , m_vlcPlayer(nullptr)
    , m_renderWindow(new QWindow())
{
    m_renderWidget = QWidget::createWindowContainer(m_renderWindow);
}

VlcPlayerImpl::~VlcPlayerImpl()
{
    if (m_vlcInstance)
        libvlc_release(m_vlcInstance);
}

bool VlcPlayerImpl::init()
{
    const char* const vlc_args[] = {
        "extraintf", "http"
    };

    m_vlcInstance = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
    if (!m_vlcInstance)
        return false;

    libvlc_media_t* vlcMedia = libvlc_media_new_path(m_vlcInstance, qtu(QString("xd.m3u8")));
    if (!vlcMedia)
        return false;

    m_vlcPlayer = libvlc_media_player_new_from_media(vlcMedia);
    if (!m_vlcPlayer)
        return false;

#if defined(Q_OS_MAC)
    libvlc_media_player_set_nsobject(m_vlcPlayer, (void*)m_renderWindow->winId());
#elif defined(Q_OS_UNIX)
    libvlc_media_player_set_xwindow(m_vlcPlayer, m_renderWindow->winId());
#elif defined(Q_OS_WIN)
    libvlc_media_player_set_hwnd(m_vlcPlayer, m_renderWindow->winId());
#endif

    libvlc_media_player_play(m_vlcPlayer);

    return true;
}

void VlcPlayerImpl::load(const QString& path)
{
}

QString VlcPlayerImpl::currentPath()
{
}

void VlcPlayerImpl::setVolume(int vol)
{
}

int VlcPlayerImpl::volume() const
{
}
}
