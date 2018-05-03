#ifndef CONTROLSWIDGET_HPP
#define CONTROLSWIDGET_HPP

#include <QWidget>

namespace Ui {
class ControlsWidget;
}

class PlayerWidget;

class ControlsWidget : public QWidget {
    Q_OBJECT

public:
    explicit ControlsWidget(PlayerWidget*);
    ~ControlsWidget();

private:
    Ui::ControlsWidget* m_ui;
};

#endif // CONTROLSWIDGET_HPP
