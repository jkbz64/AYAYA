#include "initwidget.hpp"

QSettings InitWidget::m_initSettings("init");

InitWidget::InitWidget(QWidget* parent)
    : QWidget(parent)
{
    connect(this, &InitWidget::startedIniting, this, &InitWidget::onStartedIniting);
}

InitWidget::~InitWidget() = default;

QSettings& InitWidget::initSettings()
{
    return m_initSettings;
}

bool InitWidget::isFulfilled(const QString& key)
{
    return m_initStatus.value(key, false);
}

void InitWidget::setRequirementFulfilled(const QString& key)
{
    m_initStatus[key] = true;
}

bool InitWidget::checkInitStatus()
{
    return true;
}

void InitWidget::tryToEndInit()
{
    if (checkInitStatus())
        emit endedIniting();
}

void InitWidget::onStartedIniting()
{
    init();
}
