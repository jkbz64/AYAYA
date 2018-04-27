#ifndef CONTROLSWIDGET_HPP
#define CONTROLSWIDGET_HPP

#include <QWidget>

namespace Ui {
class ControlsWidget;
}

class ControlsWidget : public QWidget {
    Q_OBJECT

public:
    explicit ControlsWidget(QWidget* parent = 0);
    ~ControlsWidget();

private:
    Ui::ControlsWidget* m_ui;
};

#endif // CONTROLSWIDGET_HPP
