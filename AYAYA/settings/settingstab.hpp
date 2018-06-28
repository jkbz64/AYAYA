#ifndef SETTINGSTAB_HPP
#define SETTINGSTAB_HPP

#include <QSettings>
#include <QWidget>

class SettingsTab : public QWidget {
    Q_OBJECT
public:
    explicit SettingsTab(QWidget* = nullptr);
    virtual ~SettingsTab();

    virtual QString tabName() const = 0;

protected:
    friend class SettingsWidget;
    virtual void updateSettings() = 0;
    virtual void applyChanges() = 0;
};

#endif // SETTINGSTAB_HPP
