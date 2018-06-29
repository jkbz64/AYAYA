#ifndef VLCPLAYERIMPL_HPP
#define VLCPLAYERIMPL_HPP

#include "playerimpl.hpp"

class libvlc_instance_t;
class libvlc_media_player_t;

class QWindow;

namespace detail {
class VlcPlayerImpl : public PlayerImpl {
    Q_OBJECT
public:
    VlcPlayerImpl(PlayerWidget*);
    virtual ~VlcPlayerImpl();

    virtual bool init() override;
    virtual void load(const QString&) override;
    virtual QString currentPath() override;
    virtual void setVolume(int) override;
    virtual int volume() const override;

private:
    libvlc_instance_t* m_vlcInstance;
    libvlc_media_player_t* m_vlcPlayer;
    QWindow* m_renderWindow;
};
}

#endif
