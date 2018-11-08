#include "extractorsettings.hpp"
#include "ui_extractorsettings.h"

#include "../player/playerwidget.hpp"

#include <QDialog>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QMetaEnum>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressBar>
#include <QPushButton>

namespace {
using ExtractorBackend = PlayerWidget::ExtractorBackend;
ExtractorBackend getBackendType(const QString& backendName)
{
    if (backendName == "Null")
        return ExtractorBackend::Null;
    if (backendName == "Youtube-Dl")
        return ExtractorBackend::Ytdl;
    if (backendName == "Streamlink")
        return ExtractorBackend::Streamlink;
    return ExtractorBackend::Null;
}

QNetworkAccessManager* m_ftp = nullptr;

void setupFtp()
{
    if (!m_ftp) {
        m_ftp = new QNetworkAccessManager();
    }
}

QNetworkReply* getYtdl()
{
    setupFtp();
#if defined(Q_OS_WIN)
    QNetworkRequest request(QUrl("https://yt-dl.org/downloads/latest/youtube-dl.exe"));
#else
    QNetworkRequest request(QUrl("https://yt-dl.org/downloads/latest/youtube-dl"));
#endif
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    return m_ftp->get(request);
}

QNetworkReply* getStreamlink()
{
    setupFtp();
    // TODO
}

QDialog* getExtractorDialog(const ExtractorBackend extractor, QNetworkReply* reply, QWidget* parent = nullptr)
{
    QDialog* dialog = new QDialog(parent);

    auto mainButton = new QPushButton("Cancel", dialog);
    auto progressBar = new QProgressBar(dialog);
    auto layout = new QVBoxLayout(dialog);
    layout->addWidget(new QLabel("Downloading " + QString(QMetaEnum::fromType<ExtractorBackend>().valueToKey(static_cast<int>(extractor)))));
    layout->addWidget(progressBar);
    layout->addWidget(mainButton);
    dialog->setLayout(layout);

    dialog->connect(mainButton, &QPushButton::pressed, [dialog, reply]() {
        dialog->disconnect(reply);
        dialog->reject();
    });

    dialog->connect(reply, &QNetworkReply::downloadProgress, [progressBar](qint64 received, qint64 total) {
        progressBar->setMinimum(0);
        progressBar->setMaximum(total);
        progressBar->setValue(received);
    });

    dialog->connect(reply, &QNetworkReply::finished, [dialog, reply]() {
        dialog->disconnect(reply);
        dialog->accept();
    });

    return dialog;
}
}

ExtractorSettings::ExtractorSettings(PlayerWidget* player, QWidget* parent)
    : SettingsTab(parent)
    , m_ui(new Ui::ExtractorSettings)
    , m_playerWidget(player)
{
    m_ui->setupUi(this);

    connect(m_ui->m_setExtractorButton, &QPushButton::released, this, &SettingsTab::settingChanged);

    connect(player, &PlayerWidget::extractorChanged, [this](ExtractorBackend backend) {
        m_ui->m_extractorLabel->setText(QMetaEnum::fromType<ExtractorBackend>().valueToKey(static_cast<int>(backend)));
    });
}

ExtractorSettings::~ExtractorSettings()
{
    delete m_ui;
    if (m_ftp) {
        delete m_ftp;
        m_ftp = nullptr;
    }
}

QString ExtractorSettings::tabName() const
{
    return QString("Extractor");
}

void ExtractorSettings::updateSettings()
{
    QSettings settings("AYAYA");
    settings.beginGroup("extractor");

    const auto backend = settings.value("backend", QVariant::fromValue(ExtractorBackend::Null));
    m_ui->m_extractorComboBox->setCurrentIndex(backend.toInt());

    const auto prefferedQuality = settings.value("prefferedQuality", "Best").toString();
    m_ui->m_prefferedQualityComboBox->setCurrentText(prefferedQuality);
    if (prefferedQuality == "Custom")
        m_ui->m_customPrefferedQualityLineEdit->setText(settings.value("customQuality").toString());

    settings.endGroup();
}

#include "../player/extractors/ytdlextractor.hpp"

void ExtractorSettings::applyChanges()
{
    QSettings settings("AYAYA");
    settings.beginGroup("extractor");

    const auto prefferedQuality = m_ui->m_prefferedQualityComboBox->currentText();
    if (prefferedQuality != "Custom") {
        settings.setValue("prefferedQuality", prefferedQuality);
        settings.remove("customQuality");
    } else {
        const auto customQuality = m_ui->m_customPrefferedQualityLineEdit->text();
        settings.setValue("customQuality", customQuality);
    }

    auto backend = getBackendType(m_ui->m_extractorComboBox->currentText());
    if (backend == ExtractorBackend::Ytdl && !YtdlExtractor::isAvailable()) {
        int ret = QMessageBox::warning(this,
            "Youtube-Dl has not been detected",
            "AYAYA couldn't find Youtube-Dl, do you want to download it for you?",
            QMessageBox::Ok | QMessageBox::No);
        if (ret == QMessageBox::Ok) {
            auto ytdlReply = getYtdl();
            connect(ytdlReply, &QNetworkReply::finished, [ytdlReply]() {
                const auto ytdlData = ytdlReply->readAll();
                QFile ytdlFile;
                ytdlFile.setFileName(qApp->applicationDirPath() + "/extractors/" + "youtube-dl");
#if defined(Q_OS_WIN)
                ytdlFile.setFileName(qApp->applicationDirPath() + "/extractors/" + "youtube-dl.exe");
#endif
                if (ytdlFile.open(QIODevice::WriteOnly)) {
                    ytdlFile.write(ytdlData);
                    ytdlFile.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);
                } else {
                    // TODO: Error message box or something
                }
                ytdlReply->deleteLater();
            });
            auto downloadDialog = getExtractorDialog(backend, ytdlReply, this);
            int ret = downloadDialog->exec();
            if (ret == QDialog::Rejected)
                backend = ExtractorBackend::Null;
            downloadDialog->deleteLater();
        } else
            backend = ExtractorBackend::Null;
    }

    settings.setValue("backend", QVariant::fromValue(backend).toInt());
    m_playerWidget->setExtractor(backend);

    settings.endGroup();
}
