#include "controlswidget.hpp"
#include "playerwidget.hpp"
#include "ui_controlswidget.h"

ControlsWidget::ControlsWidget(PlayerWidget* player)
    : QWidget(player)
    , m_ui(new Ui::ControlsWidget)
{
    m_ui->setupUi(this);
}

ControlsWidget::~ControlsWidget()
{
    delete m_ui;
}
