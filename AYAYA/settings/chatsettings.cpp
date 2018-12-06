#include "chatsettings.hpp"
#include "../chat/chatview.hpp"
#include "../chat/chatwidget.hpp"
#include "ui_chatsettings.h"
#include <QColorDialog>

ChatSettings::ChatSettings(ChatWidget* chatWidget, QWidget* parent)
    : SettingsTab(parent)
    , m_ui(new Ui::ChatSettings)
    , m_chatWidget(chatWidget)
{
    m_ui->setupUi(this);

    connect(m_ui->m_backgroundColorButton, &QPushButton::released, this, &ChatSettings::onColorButtonPressed);
    connect(m_ui->m_textColorButton, &QPushButton::released, this, &ChatSettings::onColorButtonPressed);

    connect(m_ui->m_fontComboBox, &QFontComboBox::currentFontChanged, this, &ChatSettings::settingChanged);
}

ChatSettings::~ChatSettings()
{
    delete m_ui;
}

QString ChatSettings::tabName() const
{
    return QString("Chat");
}

void ChatSettings::updateSettings()
{
    QSettings settings("AYAYA");
    settings.beginGroup("chat");

    const QColor backgroundColor = settings.value("backgroundColor", QColor::fromRgb(0, 0, 0, 255)).value<QColor>();
    m_ui->m_backgroundColorButton->setText(backgroundColor.name());
    m_ui->m_backgroundColorButton->setStyleSheet("background-color: " + backgroundColor.name());

    const QFont textFont = settings.value("textFont", QApplication::font()).value<QFont>();
    m_ui->m_fontComboBox->setCurrentFont(textFont);

    const QColor textColor = settings.value("textColor", QColor::fromRgb(255, 0, 0, 255)).value<QColor>();
    m_ui->m_textColorButton->setText(textColor.name());
    m_ui->m_textColorButton->setStyleSheet("background-color: " + textColor.name());

    settings.endGroup();
}

#include <QDebug>

void ChatSettings::applyChanges()
{
    QSettings settings("AYAYA");
    settings.beginGroup("chat");

    const QColor backgroundColor = QColor(m_ui->m_backgroundColorButton->text());
    settings.setValue("backgroundColor", backgroundColor);

    const QFont textFont = m_ui->m_fontComboBox->currentFont();
    settings.setValue("textFont", textFont);

    const QColor textColor = QColor(m_ui->m_textColorButton->text());
    settings.setValue("textColor", textColor);

    m_chatWidget->chatView()->setStyleSheet("background-color: " + backgroundColor.name() + "; color: " + textColor.name() + ";");
    m_chatWidget->chatView()->setFont(textFont);

    settings.endGroup();
}

void ChatSettings::onClearEmotesCachePressed()
{
}

void ChatSettings::onClearBadgesCachePressed()
{
}

void ChatSettings::onColorButtonPressed()
{
    auto button = qobject_cast<QPushButton*>(sender());
    const auto color = QColorDialog::getColor(QColor(button->text()), nullptr);
    if (color.isValid()) {
        button->setStyleSheet("background-color: " + color.name());
        button->setText(color.name());
        emit settingChanged();
    }
}
