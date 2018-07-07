#ifndef STREAMEXTRACTOR_HPP
#define STREAMEXTRACTOR_HPP

#include <QFuture>
#include <QObject>

using StreamFormat = QString;
using StreamFormats = QVector<StreamFormat>;

template <class T>
using ExtractorReply = QFutureWatcher<T>;

class StreamExtractor : public QObject {
    Q_OBJECT
public:
    StreamExtractor(QObject*);
    virtual ~StreamExtractor();

    virtual ExtractorReply<StreamFormats>* getStreamFormats(const QUrl&) = 0;
    virtual ExtractorReply<QUrl>* getStreamUrl(const QString&, const StreamFormat& = StreamFormat()) = 0;
};

#endif // STREAMEXTRACTOR_HPP
