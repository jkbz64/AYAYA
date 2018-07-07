#ifndef VLCPLAYERIMPL_HPP
#define VLCPLAYERIMPL_HPP

#include "playerimpl.hpp"

#include <QOpenGLWidget>

class QOpenGLTexture;
class QOpenGLFrameBufferObject;
class QOpenGLShaderProgram;

class libvlc_instance_t;
class libvlc_media_player_t;

class QWindow;

namespace detail {
class VlcPlayerImpl : public PlayerImpl {
    Q_OBJECT
public:
    VlcPlayerImpl(PlayerWidget*);
    virtual ~VlcPlayerImpl();

    virtual bool init() override;
    virtual void load(const QString&) override;
    virtual QString currentPath() override;
    virtual void setVolume(int) override;
    virtual int volume() const override;

private:
    libvlc_instance_t* m_vlcInstance;
    libvlc_media_player_t* m_vlcPlayer;
};

class PixelBuffer {
public:
    PixelBuffer(const QSize& size, int bpp = 3);
    ~PixelBuffer();

    bool shouldUpdate() const;
    void setUpdate(bool);

    unsigned char* data() const;

private:
    unsigned char* m_data;
    bool m_shouldUpdate;
};

class VlcWidget : public QWidget {
    Q_OBJECT
public:
    VlcWidget(VlcPlayerImpl*);
    ~VlcWidget();

    PixelBuffer& pixelBuffer();

protected:
    virtual void paintEvent(QPaintEvent*) override;

private:
    VlcPlayerImpl* m_impl;
    PixelBuffer m_buffer;
    QImage m_image;
};
}

#endif
