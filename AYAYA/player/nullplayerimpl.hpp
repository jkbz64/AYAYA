#ifndef NULLPLAYERIMPL_HPP
#define NULLPLAYERIMPL_HPP

#include "playerimpl.hpp"

namespace detail {
class NullPlayerImpl : public PlayerImpl {
    Q_OBJECT
public:
    NullPlayerImpl(PlayerWidget*);
    virtual ~NullPlayerImpl();

    virtual bool init() override;
    virtual void load(const QString&) override;
    virtual QString currentPath() override;
    virtual void setVolume(int) override;
    virtual int volume() const override;
};
}

#endif // NULLPLAYERIMPL_HPP
