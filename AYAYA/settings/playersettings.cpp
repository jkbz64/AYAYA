#include "playersettings.hpp"
#include "ui_playersettings.h"

#include "../player/playerwidget.hpp"

#include <QMetaEnum>
#include <QSpinBox>

#ifdef MPV
#include "../player/backends/mpvplayerimpl.hpp"
#endif

namespace {
using PlayerBackend = PlayerWidget::Backend;
PlayerBackend getBackendType(const QString& backendName)
{
    if (backendName == "Null")
        return PlayerBackend::Null;
    if (backendName == "Mpv")
        return PlayerBackend::Mpv;
    if (backendName == "Vlc")
        return PlayerBackend::Vlc;
    return PlayerBackend::Null;
}
QString getBackendName(const PlayerBackend backend)
{
    return QString(QMetaEnum::fromType<PlayerBackend>().valueToKey(static_cast<int>(backend)));
}
}

PlayerSettings::PlayerSettings(PlayerWidget* playerWidget, QWidget* parent)
    : SettingsTab(parent)
    , m_ui(new Ui::PlayerSettings)
    , m_playerWidget(playerWidget)
{
    m_ui->setupUi(this);

    m_ui->m_backendComboBox->addItem("Null");
    connect(m_ui->m_backendComboBox, &QComboBox::currentTextChanged, this, &PlayerSettings::onBackendChanged);
    m_ui->m_mpvCustomTableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

#ifdef MPV
    m_ui->m_backendComboBox->addItem("Mpv");
    connect(m_ui->m_mpvCustomTableWidget, &QTableWidget::customContextMenuRequested, this, &PlayerSettings::onRequestedMpvCustomMenu);
    connect(m_ui->m_mpvCustomTableWidget, &QTableWidget::itemChanged, this, &PlayerSettings::settingChanged);
#endif
#ifdef VLC
    m_ui->m_backendComboBox->addItem("Vlc");
#endif

    connect(m_ui->m_setCurrentBackendButton, &QPushButton::pressed, this, &PlayerSettings::onCurrentBackendChanged);
    connect(m_ui->m_defaultVolumeSpinBox, &QSpinBox::editingFinished, this, &PlayerSettings::settingChanged);
    connect(m_ui->m_hideControlsSpinBox, &QSpinBox::editingFinished, this, &PlayerSettings::settingChanged);

    connect(playerWidget, &PlayerWidget::backendChanged, [this](PlayerBackend backend) {
        m_ui->m_backendLabel->setText(QMetaEnum::fromType<PlayerBackend>().valueToKey(static_cast<int>(backend)));
    });
}

PlayerSettings::~PlayerSettings()
{
    delete m_ui;
}

QString PlayerSettings::tabName() const
{
    return QString("Player");
}

void PlayerSettings::updateSettings()
{
    QSettings settings("AYAYA");
    settings.beginGroup("player");

    // Load current backend
    const auto currentBackend = settings.value("backend", QVariant::fromValue(PlayerBackend::Null));
    m_ui->m_backendLabel->setText(getBackendName(currentBackend.value<PlayerBackend>()));
    m_ui->m_backendComboBox->setCurrentIndex(currentBackend.toInt());

    // Default volume
    const auto volume = settings.value("defaultVolume", 50).toInt();
    m_ui->m_defaultVolumeSpinBox->setValue(volume);

    const auto hideControlsTimeout = settings.value("hideControlsTimeout", 1500).toInt();
    m_ui->m_hideControlsSpinBox->setValue(hideControlsTimeout);

#ifdef MPV
    settings.beginGroup("mpv");
    const auto mpvCustomOptions = settings.value("custom").toMap();
    int i = 0;
    for (auto it = mpvCustomOptions.cbegin(); it != mpvCustomOptions.cend(); ++it) {
        m_ui->m_mpvCustomTableWidget->insertRow(m_ui->m_mpvCustomTableWidget->rowCount());
        m_ui->m_mpvCustomTableWidget->setItem(i, 0, new QTableWidgetItem(it.key()));
        m_ui->m_mpvCustomTableWidget->setItem(i, 1, new QTableWidgetItem(it.value().toString()));
        ++i;
    }
    settings.endGroup();
#endif

#ifdef VLC
    settings.beginGroup("vlc");

    settings.endGroup();
#endif

    settings.endGroup();
}

void PlayerSettings::applyChanges()
{
    QSettings settings("AYAYA");
    settings.beginGroup("player");

    // Volume
    const auto volume = m_ui->m_defaultVolumeSpinBox->value();
    settings.setValue("defaultVolume", volume);
    m_playerWidget->setDefaultVolume(volume);

    const auto hideControlsTimeout = static_cast<int>(m_ui->m_hideControlsSpinBox->value());
    settings.setValue("hideControlsTimeout", hideControlsTimeout);
    m_playerWidget->setHideControlsTimeout(hideControlsTimeout);

    const auto backendName = m_ui->m_backendLabel->text();
    settings.setValue("backend", QVariant::fromValue(getBackendType(backendName)).toInt());
    m_playerWidget->setBackend(getBackendType(backendName));

#ifdef MPV
    settings.beginGroup("mpv");

    QVariantMap mpvSettings;
    for (auto i = 0; i < m_ui->m_mpvCustomTableWidget->rowCount(); ++i) {
        const auto key = m_ui->m_mpvCustomTableWidget->item(i, 0) ? m_ui->m_mpvCustomTableWidget->item(i, 0)->text() : QString("");
        const auto value = m_ui->m_mpvCustomTableWidget->item(i, 1) ? m_ui->m_mpvCustomTableWidget->item(i, 1)->text() : QString("");
        mpvSettings.insert(key, value);
    }
    settings.setValue("custom", mpvSettings);
    settings.endGroup();

    if (m_playerWidget->backend() == PlayerWidget::Backend::Mpv) { // Apply settings to the backend
        for (auto i = mpvSettings.begin(); i != mpvSettings.end(); ++i)
            if (!i.key().isEmpty() && i.value().isValid())
                qobject_cast<detail::MpvPlayerImpl*>(m_playerWidget->m_impl)->setOption(i.key(), i.value());
    }
#endif

#ifdef VLC
    settings.beginGroup("vlc");
    // TODO
    settings.endGroup();
#endif

    settings.endGroup();
}

void PlayerSettings::onCurrentBackendChanged()
{
    m_ui->m_backendLabel->setText(m_ui->m_backendComboBox->currentText());
    emit settingChanged();
}

void PlayerSettings::onBackendChanged(const QString& backendName)
{
    const auto backendType = getBackendType(backendName);
    switch (backendType) {
    case PlayerBackend::Null:
        m_ui->m_specificStackedWidget->setCurrentWidget(m_ui->m_nullSpecificWidget);
        break;
    case PlayerBackend::Mpv:
        m_ui->m_specificStackedWidget->setCurrentWidget(m_ui->m_mpvSpecificWidget);
        break;
    case PlayerBackend::Vlc:
        m_ui->m_specificStackedWidget->setCurrentWidget(m_ui->m_vlcSpecificWidget);
        break;
    }
}

#include <QMenu>

void PlayerSettings::onRequestedMpvCustomMenu(const QPoint& pos)
{
    auto menu = new QMenu(this);
    menu->addAction("Add", [this]() {
        m_ui->m_mpvCustomTableWidget->insertRow(m_ui->m_mpvCustomTableWidget->rowCount());
        m_ui->m_mpvCustomTableWidget->editItem(m_ui->m_mpvCustomTableWidget->item(m_ui->m_mpvCustomTableWidget->rowCount() - 1, 0));
    });
    menu->addAction("Delete", [this]() {
        m_ui->m_mpvCustomTableWidget->removeRow(m_ui->m_mpvCustomTableWidget->currentRow());
        emit settingChanged();
    });

    const auto mappedPos = m_ui->m_mpvCustomTableWidget->mapToGlobal(pos);
    menu->exec(mappedPos);
}
