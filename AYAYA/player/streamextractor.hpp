#ifndef STREAMEXTRACTOR_HPP
#define STREAMEXTRACTOR_HPP

#include <QFuture>
#include <QObject>
#include <QVector>

using StreamFormat = QString;
using StreamFormats = QVector<StreamFormat>;

using StreamFormatsReply = QFutureWatcher<StreamFormats>;
using StreamUrlReply = QFutureWatcher<QUrl>;

class StreamExtractor : public QObject {
    Q_OBJECT
public:
    StreamExtractor(QObject*);
    virtual ~StreamExtractor();

    virtual StreamFormatsReply* getStreamFormats(const QUrl&) = 0;
    virtual StreamUrlReply* getStreamUrl(const QString&, const StreamFormat& = StreamFormat()) = 0;
};

#endif // STREAMEXTRACTOR_HPP
