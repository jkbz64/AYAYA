#include "settingswidget.hpp"
#include "ui_settingswidget.h"

#include "settings/quickstartdialog.hpp"
#include "settings/settingstab.hpp"

#include <QFile>
#include <QStandardPaths>

SettingsWidget::SettingsWidget(QWidget* parent)
    : InitWidget(parent)
    , m_ui(new Ui::SettingsWidget)
{
    m_ui->setupUi(this);
    m_ui->m_settingsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

SettingsWidget::~SettingsWidget()
{
    delete m_ui;
}

#include "settings/quickstartdialog.hpp"

void SettingsWidget::init()
{
    QSettings settings("AYAYA");
    const bool quickstart = settings.value("quickstart", true).toBool();
    if (quickstart) {
        auto quickstartDialog = new QuickstartDialog(this);
        int ret = quickstartDialog->exec();
        if (ret == QDialog::Accepted) {
            settings.beginGroup("global");
            settings.setValue("theme", QVariant::fromValue(quickstartDialog->selectedTheme()).toInt());
            settings.endGroup();

            settings.beginGroup("extractor");
            settings.setValue("backend", QVariant::fromValue(quickstartDialog->selectedExtractorBackend()).toInt());
            settings.endGroup();

            settings.beginGroup("player");
            settings.setValue("backend", QVariant::fromValue(quickstartDialog->selectedPlayerBackend()).toInt());
            settings.endGroup();

            settings.setValue("quickstart", false);
        }
        quickstartDialog->deleteLater();
    }
    // Sync the changes in files before loading changes in specific tabs
    settings.sync();
    // Load settings from file and apply changes at the startup
    for (const auto tab : m_tabs) {
        tab->updateSettings();
        tab->applyChanges();
    }
    tryToEndInit();
}

void SettingsWidget::applyChanges()
{
    for (const auto tab : m_tabs) {
        tab->applyChanges();
    }
}

#include <QVBoxLayout>

void SettingsWidget::appendTab(SettingsTab* tab)
{
    m_tabs.push_back(tab);
    qobject_cast<QVBoxLayout*>(m_ui->m_settingsWidget->layout())->addWidget(tab);
    connect(tab, &SettingsTab::settingChanged, [tab]() { tab->applyChanges(); });
}
