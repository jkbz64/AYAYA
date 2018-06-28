#include "nullplayerimpl.hpp"

#include <QLabel>

detail::NullPlayerImpl::NullPlayerImpl(PlayerWidget* player)
    : PlayerImpl(player)
{
    m_renderWidget = new QLabel("Null Backend. Set different backend in the settings or compile AYAYA with one of supported backends.");
}

detail::NullPlayerImpl::~NullPlayerImpl()
{
}

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
