#include "controlswidget.hpp"
#include "ui_controlswidget.h"

ControlsWidget::ControlsWidget(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::ControlsWidget)
{
    m_ui->setupUi(this);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

ControlsWidget::~ControlsWidget()
{
    delete m_ui;
}
