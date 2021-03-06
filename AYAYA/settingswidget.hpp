#ifndef SETTINGSWIDGET_HPP
#define SETTINGSWIDGET_HPP

#include "initwidget.hpp"

class SettingsTab;

class QPushButton;

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public InitWidget {
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget* parent = 0);
    ~SettingsWidget();

    virtual void init() override;

    void clear();

    template <class T, typename... Args>
    void addTab(Args&&...);

private:
    Ui::SettingsWidget* m_ui;
    QVector<SettingsTab*> m_tabs;

    void applyChanges();

    void appendTab(SettingsTab*);
};

template <class T, typename... Args>
inline void SettingsWidget::addTab(Args&&... args)
{
    appendTab(new T(args..., this));
}

#endif // SETTINGSWIDGET_HPP
