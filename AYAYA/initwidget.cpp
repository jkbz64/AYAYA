#include "initwidget.hpp"

namespace {
// I was just too lazy to check if qsettings is shared between InitWidgets, lol.
QSettings m_initSettings("initSettings", QSettings::NativeFormat);
}

InitWidget::InitWidget(QWidget* parent)
    : QWidget(parent)
{
    connect(this, &InitWidget::startedIniting, this, &InitWidget::onStartedIniting);
}

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

void InitWidget::onStartedIniting()
{
    init();
}

void InitWidget::tryToEndInit()
{
    if (checkInitStatus()) {
        emit endedIniting();
    }
}

InitWidget::~InitWidget() = default;
