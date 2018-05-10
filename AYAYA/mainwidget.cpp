#include "mainwidget.hpp"
#include <QMainWindow>

MainWidget::MainWidget(QWidget* parent)
    : QWidget(parent)
{
    connect(this, &MainWidget::startedIniting, this, &MainWidget::onStartedIniting);
    // Not so sure if it will be called after or before MainWindow, in most cases it calls after call to MainWindow
    // because of DirectConnection and this being QueuedConnection.. Need to fix it somehow later
    connect(this, &MainWidget::endedIniting, this, &MainWidget::onEndedIniting, Qt::QueuedConnection);
}

void MainWidget::onStartedIniting()
{
    init();
}

void MainWidget::onEndedIniting()
{
    disconnect(this, &MainWidget::startedIniting, 0, 0);
    disconnect(this, &MainWidget::initProgress, 0, 0);
    disconnect(this, &MainWidget::endedIniting, 0, 0);
}

MainWidget::~MainWidget() = default;
