#include "vlcplayerimpl.hpp"
#include "playerwidget.hpp"
#include <QOpenGLWidget>
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
    m_renderWindow->setFlags(Qt::FramelessWindowHint | Qt::WindowTransparentForInput);
    m_renderWidget = QWidget::createWindowContainer(m_renderWindow, player);
    m_renderWidget->setStyleSheet("background-color: rgba(0, 0, 0, 0);");
}

VlcPlayerImpl::~VlcPlayerImpl()
{
    if (m_vlcInstance)
        libvlc_release(m_vlcInstance);
}

bool VlcPlayerImpl::init()
{
    const char* const vlc_args[] = {
        "-I=core,http"
    };
    m_vlcInstance = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
    if (!m_vlcInstance)
        return false;

    libvlc_media_t* vlcMedia = libvlc_media_new_location(m_vlcInstance, qtu(QString("https://video-weaver.fra02.hls.ttvnw.net/v1/playlist/CvgCLUu_6MlUMERbS6JA4TSWBEKObr7cGzYV6MK-7DieFoeFD07Cd9KAhASrikBUyQxHcvtlGYrLYPf1RcA1DOrphivmmMF2oEAW26z6mcYGsiGctd36_S96FEQZEDAK-VPwPrBjrYjCUn_A0uIgATwmSnc40I73TeiBitsX-4RB8sSFldAsWKZ8KTdz3_dAY2JsSNAjSOsBbutLHv6U3tPpb-mvjlMXPAQ4qgUIkCbMdWBMVIjokaD0QFMDXhphtGCs7OoNYjOWWi4YWR-hIfD-CwdNEvqEFiH6jPQRD10ueXq4yfnelFz_kl0qMsAbZXrm15eaZAMAsCEHIudm121mPss13SlcfY6yezksOLp3oXWfa8BznEn2sIQwzEZ2HE7l77O9l2aP-diSVuMGigr_sk4L2SkpaV8VS3kkdJsyqATdJCZRTQthav-GdO9f-JMoAZ8AdRx0iU0S3jM6lxmjM0_MEEWd-vlw_zNEKSW9izYLV9pVJ8bWfRIQhzqUpOGrNlXyMYJ6TGkOOhoMiOyXD2KfCxlbXZMs.m3u8")));
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
