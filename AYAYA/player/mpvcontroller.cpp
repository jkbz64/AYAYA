#include "mpvcontroller.hpp"

#include <QWidget>

namespace {
static void wakeup(void* ctx)
{
    // This callback is invoked from any mpv thread (but possibly also
    // recursively from a thread that is calling the mpv API). Just notify
    // the Qt GUI thread to wake up (so that it can process events with
    // mpv_wait_event()), and return as quickly as possible.
    MpvController* mpvObject = (MpvController*)ctx;
    emit mpvObject->mpvEvent();
}
}

MpvController::MpvController(mpv::qt::Handle handle)
    : PlayerController()
    , m_mpv(handle)
{
}

MpvController::~MpvController() = default;

bool MpvController::init()
{
    connect(this, &MpvController::mpvEvent, this, &MpvController::processEvents, Qt::QueuedConnection);

    std::setlocale(LC_NUMERIC, "C");

    if (!m_mpv)
        throw std::runtime_error("could not create mpv context");

    // mpv_set_option_string(m_mpv, "terminal", "yes");
    // mpv_set_option_string(m_mpv, "msg-level", "all=v");
    mpv_set_option_string(m_mpv, "keepaspect", "no");

    mpv_observe_property(m_mpv, 0, "playback-time", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "volume", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "cache-buffering-state", MPV_FORMAT_INT64);

    mpv_set_wakeup_callback(m_mpv, wakeup, this);

    if (mpv_initialize(m_mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");

    return true;
}

mpv::qt::Handle& MpvController::handle()
{
    return m_mpv;
}

void MpvController::command(const QVariant& params)
{
    mpv::qt::command_variant(m_mpv, params);
}

void MpvController::setProperty(const QString& name, const QVariant& value)
{
    mpv::qt::set_property_variant(m_mpv, name, value);
}

const QVariant MpvController::getProperty(const QString& name) const
{
    return mpv::qt::get_property_variant(m_mpv, name);
}

void MpvController::load(const QString& path)
{
    command((QStringList() << "loadfile"
                           << path));
}

QString MpvController::currentPath()
{
    return getProperty("path").toString();
}

void MpvController::setVolume(int value)
{
    setOption("volume", value);
}

void MpvController::mute(bool mute)
{
    const QString shouldMute = mute ? "yes" : "no";
    setOption("mute", shouldMute);
}

int MpvController::volume() const
{
    return getProperty("volume").toInt();
}

void MpvController::setOption(const QString& name, const QVariant& value)
{
    mpv::qt::set_option_variant(m_mpv, name, value);
}

void MpvController::processEvents()
{
    while (m_mpv) {
        mpv_event* event = mpv_wait_event(m_mpv, 0);
        if (event->event_id == MPV_EVENT_NONE)
            break;
        handleMpvEvent(event);
    }
}

void MpvController::handleMpvEvent(mpv_event* event)
{
    switch (event->event_id) {
    case MPV_EVENT_PROPERTY_CHANGE: {
        mpv_event_property* prop = (mpv_event_property*)event->data;
        if (strcmp(prop->name, "playback-time") == 0) {
            if (prop->format == MPV_FORMAT_DOUBLE)
                emit positionChanged(*(double*)prop->data);
        }
        if (strcmp(prop->name, "volume") == 0) {
            if (prop->format == MPV_FORMAT_DOUBLE)
                emit volumeChanged(*(double*)prop->data);
        }
        if (strcmp(prop->name, "cache-buffering-state") == 0) {
            if (prop->format == MPV_FORMAT_INT64) {
                if ((int)*(int*)prop->data < 100)
                    emit buffering(*(int*)prop->data);
            }
        }
        break;
    }
    case MPV_EVENT_VIDEO_RECONFIG:
        // TODO
        break;
    case MPV_EVENT_END_FILE:
        emit ended();
        break;
    case MPV_EVENT_START_FILE:
        emit startedLoading();
        break;
    case MPV_EVENT_FILE_LOADED:
        emit loaded();
        break;
    case MPV_EVENT_LOG_MESSAGE: {
        struct mpv_event_log_message* msg = (struct mpv_event_log_message*)event->data;

        break;
    }
    case MPV_EVENT_SHUTDOWN: {
        break;
    }
    default:;
        // Ignore uninteresting or unknown events.
    }
}
