#ifndef YTDLEXTRACTOR_HPP
#define YTDLEXTRACTOR_HPP

#include "../streamextractor.hpp"

class QProcess;

class YtdlExtractor : public StreamExtractor {
    Q_OBJECT
public:
    YtdlExtractor(QObject* = nullptr);
    ~YtdlExtractor();

    virtual ExtractorReply<StreamFormats>* getStreamFormats(const QUrl&) override;
    virtual ExtractorReply<QUrl>* getStreamUrl(const QString&, const StreamFormat&) override;
};

#endif
