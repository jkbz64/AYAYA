#ifndef CHATSETTINGS_HPP
#define CHATSETTINGS_HPP

#include "settingstab.hpp"

class ChatWidget;

namespace Ui {
class ChatSettings;
}

class ChatSettings : public SettingsTab {
    Q_OBJECT

public:
    ChatSettings(ChatWidget*, QWidget* = nullptr);
    ~ChatSettings();

    virtual QString tabName() const override;

protected:
    virtual void updateSettings() override;
    virtual void applyChanges() override;

private:
    Ui::ChatSettings* m_ui;
    ChatWidget* m_chatWidget;

private slots:
    void onClearEmotesCachePressed();
    void onClearBadgesCachePressed();

    void onColorButtonPressed();
};

#endif // CHATSETTINGS_HPP
