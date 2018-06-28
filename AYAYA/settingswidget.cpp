#include "settingswidget.hpp"
#include "settings/settingstab.hpp"
#include "ui_settingswidget.h"

SettingsWidget::SettingsWidget(QWidget* parent)
    : InitWidget(parent)
    , m_ui(new Ui::SettingsWidget)
{
    m_ui->setupUi(this);

    connect(m_ui->m_saveButton, &QPushButton::pressed, this, &SettingsWidget::onApplyPressed);
}

SettingsWidget::~SettingsWidget()
{
    delete m_ui;
}

void SettingsWidget::init()
{
    const auto tabCount = m_ui->m_settingsTabs->count();
    auto i = 0;
    while (i < tabCount) {
        const auto tab = qobject_cast<SettingsTab*>(m_ui->m_settingsTabs->widget(i++));
        tab->updateSettings();
        tab->applyChanges();
    }
    tryToEndInit();
}

void SettingsWidget::clear()
{
    m_ui->m_settingsTabs->clear();
}

QPushButton* SettingsWidget::saveButton() const
{
    return m_ui->m_saveButton;
}

void SettingsWidget::appendTab(SettingsTab* tab)
{
    m_ui->m_settingsTabs->addTab(tab, tab->tabName());
    tab->updateSettings();
}

void SettingsWidget::onApplyPressed()
{
    const auto tabCount = m_ui->m_settingsTabs->count();
    auto i = 0;
    while (i < tabCount) {
        const auto tab = qobject_cast<SettingsTab*>(m_ui->m_settingsTabs->widget(i++));
        tab->applyChanges();
    }
    emit saved();
}
