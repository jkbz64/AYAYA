#include "nullplayerimpl.hpp"

detail::NullPlayerImpl::NullPlayerImpl(PlayerWidget* player)
    : PlayerImpl(player)
{
}

detail::NullPlayerImpl::~NullPlayerImpl() = default;

bool detail::NullPlayerImpl::init()
{
    return true;
}

void detail::NullPlayerImpl::load(const QString&)
{
}

QString detail::NullPlayerImpl::currentPath()
{
    return QString();
}

void detail::NullPlayerImpl::setVolume(int)
{
}

int detail::NullPlayerImpl::volume() const
{
    return 0;
}
