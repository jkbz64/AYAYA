#include "settingswidget.hpp"
#include "settings/settingstab.hpp"
#include "ui_settingswidget.h"

SettingsWidget::SettingsWidget(QWidget* parent)
    : InitWidget(parent)
    , m_ui(new Ui::SettingsWidget)
{
    m_ui->setupUi(this);

    connect(exitButton(), &QPushButton::pressed, this, &SettingsWidget::onExitPressed);
    connect(saveButton(), &QPushButton::pressed, this, &SettingsWidget::onSavePressed);
}

SettingsWidget::~SettingsWidget()
{
    delete m_ui;
}

void SettingsWidget::init()
{

    for (const auto tab : m_tabs) {
        tab->updateSettings();
        tab->applyChanges();
    }
    tryToEndInit();
}

void SettingsWidget::clear()
{
    m_ui->m_settingsTabs->clear();
}

QPushButton* SettingsWidget::exitButton() const
{
    return m_ui->m_exitButton;
}

QPushButton* SettingsWidget::saveButton() const
{
    return m_ui->m_saveButton;
}

void SettingsWidget::appendTab(SettingsTab* tab)
{
    m_ui->m_settingsTabs->addTab(tab, tab->tabName());
    m_tabs.push_back(tab);
}

void SettingsWidget::onExitPressed()
{
    emit exited();
}

void SettingsWidget::onSavePressed()
{
    for (const auto tab : m_tabs)
        tab->applyChanges();
    emit saved();
}
