#include "playersettings.hpp"
#include "ui_playersettings.h"

#include "../player/playerwidget.hpp"

namespace {
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
#endif
#ifdef VLC
    m_ui->m_backendComboBox->addItem("Vlc");
#endif
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
    const auto currentBackend = settings.value("backend", "Null").toString();
    m_ui->m_backendLabel->setText(currentBackend);
    m_ui->m_backendComboBox->setCurrentText(currentBackend);

    // Preffered Quality
    const auto prefferedQuality = settings.value("prefferedQuality", "Best").toString();
    m_ui->m_prefferedQualityComboBox->setCurrentText(prefferedQuality);
    if (prefferedQuality == "Custom")
        m_ui->m_customPrefferedQualityLineEdit->setText(settings.value("customQuality").toString());

    // Default volume
    const auto volume = settings.value("defaultVolume", 50).toInt();
    m_ui->m_defaultVolumeSpinBox->setValue(volume);

    // Keep aspect ratio
    const auto keepAspectRatio = settings.value("keepAspectRatio", true).toBool();
    m_ui->m_keepAspectRatioCheckBox->setChecked(keepAspectRatio);

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

    // Preffered Quality
    const auto prefferedQuality = m_ui->m_prefferedQualityComboBox->currentText();
    if (prefferedQuality != "Custom") {
        settings.setValue("prefferedQuality", prefferedQuality);
        settings.remove("customQuality");
    } else {
        const auto customQuality = m_ui->m_customPrefferedQualityLineEdit->text();
        settings.setValue("customQuality", customQuality);
    }

    // TODO: PlayerWidget->setPrefferedQuality()

    // Volume
    const auto volume = m_ui->m_defaultVolumeSpinBox->value();
    settings.setValue("defaultVolume", volume);
    m_playerWidget->setVolume(volume);
    settings.setValue("backend", m_ui->m_backendLabel->text());

    // Keep aspect ratio
    const auto keepAspectRation = m_ui->m_keepAspectRatioCheckBox->isChecked();
    settings.setValue("keepAspectRatio", keepAspectRation);

#ifdef MPV
    settings.beginGroup("mpv");

    QVariantMap mpvSettings;
    for (auto i = 0; i < m_ui->m_mpvCustomTableWidget->rowCount(); ++i) {
        const auto key = m_ui->m_mpvCustomTableWidget->item(i, 0)->text();
        const auto value = m_ui->m_mpvCustomTableWidget->item(i, 1)->text();
        mpvSettings.insert(key, value);
    }
    settings.setValue("custom", mpvSettings);

    settings.endGroup();
#endif

#ifdef VLC

#endif

    settings.endGroup();
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
    });
    menu->addAction("Delete", [this]() {
        m_ui->m_mpvCustomTableWidget->removeRow(m_ui->m_mpvCustomTableWidget->currentRow());
    });

    const auto mappedPos = m_ui->m_mpvCustomTableWidget->mapToGlobal(pos);
    menu->exec(mappedPos);
}
