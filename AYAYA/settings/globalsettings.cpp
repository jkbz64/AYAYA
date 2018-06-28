#include "globalsettings.hpp"
#include "ui_globalsettings.h"

#include "../mainwindow.hpp"

GlobalSettings::GlobalSettings(MainWindow* mainWindow, QWidget* parent)
    : SettingsTab(parent)
    , m_ui(new Ui::GlobalSettings)
    , m_mainWindow(mainWindow)
{
    m_ui->setupUi(this);
}

GlobalSettings::~GlobalSettings()
{
    delete m_ui;
}

QString GlobalSettings::tabName() const
{
    return QString("Global");
}

void GlobalSettings::updateSettings()
{
    QSettings settings("AYAYA");

    settings.beginGroup("global");
    const auto currentTheme = settings.value("theme", "Default").toString();
    m_ui->m_currentThemeLabel->setText(currentTheme);
    if (currentTheme == "Default")
        m_ui->m_defaultButton->setChecked(true);
    else
        m_ui->m_darkButton->setChecked(true);
    settings.endGroup();
}

void GlobalSettings::applyChanges()
{
    QSettings settings("AYAYA");

    settings.beginGroup("global");
    if (m_ui->m_defaultButton->isChecked()) {
        m_mainWindow->setTheme(Theme::Default);
        settings.setValue("theme", "Default");
    } else if (m_ui->m_darkButton->isChecked()) {
        m_mainWindow->setTheme(Theme::Dark);
        settings.setValue("theme", "Dark");
    }
    settings.endGroup();
}
