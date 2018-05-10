#include "initwidget.hpp"
#include <QMainWindow>

InitWidget::InitWidget(QWidget* parent)
    : QWidget(parent)
{
    connect(this, &InitWidget::startedIniting, this, &InitWidget::onStartedIniting);
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
