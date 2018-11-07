#include "globalsettings.hpp"
#include "ui_globalsettings.h"

#include "../mainwindow.hpp"

#include <QPushButton>

namespace {
using Theme = MainWindow::Theme;
}

GlobalSettings::GlobalSettings(MainWindow* mainWindow, QWidget* parent)
    : SettingsTab(parent)
    , m_ui(new Ui::GlobalSettings)
    , m_mainWindow(mainWindow)
{
    m_ui->setupUi(this);
    connect(m_ui->m_defaultButton, &QPushButton::released, this, &SettingsTab::settingChanged);
    connect(m_ui->m_darkButton, &QPushButton::released, this, &SettingsTab::settingChanged);
}

GlobalSettings::~GlobalSettings()
{
    delete m_ui;
}

QString GlobalSettings::tabName() const
{
    return QString("Global");
}

#include <QMetaEnum>

void GlobalSettings::updateSettings()
{
    QSettings settings("AYAYA");

    settings.beginGroup("global");
    const Theme currentTheme = settings.value("theme", QVariant::fromValue(Theme::Default)).value<Theme>();
    m_ui->m_currentThemeLabel->setText(QMetaEnum::fromType<Theme>().valueToKey(static_cast<int>(currentTheme)));
    if (currentTheme == Theme::Default)
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
        settings.setValue("theme", QVariant::fromValue(Theme::Default).toInt());
        m_ui->m_currentThemeLabel->setText("Default");
    } else if (m_ui->m_darkButton->isChecked()) {
        m_mainWindow->setTheme(Theme::Dark);
        settings.setValue("theme", QVariant::fromValue(Theme::Dark).toInt());
        m_ui->m_currentThemeLabel->setText("Dark");
    }
    settings.endGroup();
}
