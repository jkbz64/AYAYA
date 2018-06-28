#ifndef GLOBALSETTINGS_HPP
#define GLOBALSETTINGS_HPP

#include "settingstab.hpp"

class QComboBox;
class MainWindow;

namespace Ui {
class GlobalSettings;
}

class GlobalSettings : public SettingsTab {
    Q_OBJECT

public:
    explicit GlobalSettings(MainWindow*, QWidget* = nullptr);
    ~GlobalSettings();

    virtual QString tabName() const override;

protected:
    virtual void updateSettings() override;
    virtual void applyChanges() override;

private:
    Ui::GlobalSettings* m_ui;
    MainWindow* m_mainWindow;
};

#endif // GLOBALSETTINGS_HPP
