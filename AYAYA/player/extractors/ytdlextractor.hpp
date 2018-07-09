#ifndef YTDLEXTRACTOR_HPP
#define YTDLEXTRACTOR_HPP

#include "../streamextractor.hpp"

class QProcess;

class YtdlExtractor : public StreamExtractor {
    Q_OBJECT
public:
    YtdlExtractor(QObject* = nullptr);
    ~YtdlExtractor();

    virtual bool isAvailable() override;

    virtual StreamFormatsReply* getStreamFormats(const QUrl&) override;
    virtual StreamUrlReply* getStreamUrl(const QString&, const StreamFormat& = StreamFormat("best")) override;
};

#endif
