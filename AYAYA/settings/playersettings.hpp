#ifndef PLAYERSETTINGS_HPP
#define PLAYERSETTINGS_HPP

#include "settingstab.hpp"

class PlayerWidget;

namespace Ui {
class PlayerSettings;
}

class PlayerSettings : public SettingsTab {
    Q_OBJECT
public:
    PlayerSettings(PlayerWidget*, QWidget* = nullptr);
    ~PlayerSettings();

    virtual QString tabName() const override;

protected:
    virtual void updateSettings() override;
    virtual void applyChanges() override;

private:
    Ui::PlayerSettings* m_ui;
    PlayerWidget* m_playerWidget;

private slots:
    void onBackendChanged(const QString&);
    void onRequestedMpvCustomMenu(const QPoint&);
};

#endif // PLAYERSETTINGS_HPP
