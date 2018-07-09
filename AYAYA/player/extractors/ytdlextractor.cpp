#include "ytdlextractor.hpp"
#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QStandardPaths>
#include <QUrl>

namespace {
QString ytdlExecutablePath()
{
#if defined(Q_OS_WIN)
    return QDir(qApp->applicationDirPath()).filePath("extractors/youtube-dl.exe");
#elif defined(Q_OS_LINUX)
    return QDir(qApp->applicationDirPath()).filePath("extractors/youtube-dl");
#endif
}
}

YtdlExtractor::YtdlExtractor(QObject* parent)
    : StreamExtractor(parent)
{
}

YtdlExtractor::~YtdlExtractor() = default;

#include <QFutureWatcher>
#include <QRegularExpression>
#include <QtConcurrent/QtConcurrent>

bool YtdlExtractor::isAvailable()
{
    return QFile::exists(ytdlExecutablePath());
}

StreamFormatsReply* YtdlExtractor::getStreamFormats(const QUrl& url)
{
    QFuture<StreamFormats> formatsFuture = QtConcurrent::run(QThreadPool::globalInstance(), [url]() -> auto {
        QStringList args;
        args << "-F" << url.toString();

        QProcess ytdl;
        ytdl.start(ytdlExecutablePath(), args);

        if (ytdl.waitForFinished()) {
            StreamFormats formats;
            auto output = ytdl.readAll();
            QRegularExpression reg("(\\d{3,4}p\\d*)");
            QRegularExpressionMatchIterator i = reg.globalMatch(output);
            while (i.hasNext()) {
                const QRegularExpressionMatch match = i.next();
                const auto format = match.captured(0);
                formats.push_back(format);
            }
            return formats;
        }
        return StreamFormats();
    });

    auto reply = new StreamFormatsReply(this);
    reply->setFuture(formatsFuture);
    return reply;
}

StreamUrlReply* YtdlExtractor::getStreamUrl(const QString& name, const StreamFormat& format)
{
    QFuture<QUrl> urlFuture = QtConcurrent::run(QThreadPool::globalInstance(), [ name, format ]() -> auto {
        QStringList args;
        if (!format.isEmpty())
            args << "-f" << format;
        args << "-g" << name;

        QProcess ytdl;
        ytdl.start(ytdlExecutablePath(), args);

        if (ytdl.waitForFinished())
            return QUrl(ytdl.readAll());
        return QUrl();
    });

    auto reply = new StreamUrlReply(this);
    reply->setFuture(urlFuture);
    return reply;
}
