#ifndef EXTRACTORSETTINGS_HPP
#define EXTRACTORSETTINGS_HPP

#include "settingstab.hpp"
#include <QWidget>

class PlayerWidget;

namespace Ui {
class ExtractorSettings;
}

class ExtractorSettings : public SettingsTab {
    Q_OBJECT

public:
    explicit ExtractorSettings(PlayerWidget*, QWidget* parent = nullptr);
    virtual ~ExtractorSettings();

    virtual QString tabName() const override;

protected:
    virtual void updateSettings() override;
    virtual void applyChanges() override;

private:
    Ui::ExtractorSettings* m_ui;
    PlayerWidget* m_playerWidget;
};

#endif // EXTRACTORSETTINGS_HPP
