#include "mainwidget.hpp"
#include <QMainWindow>

MainWidget::MainWidget(QWidget* parent)
    : QWidget(parent)
{
    connect(this, &MainWidget::startedIniting, [this]() {
        init();
    });
}

MainWidget::~MainWidget() = default;
