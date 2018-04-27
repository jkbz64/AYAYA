#ifndef MPVPLAYER_HPP
#define MPVPLAYER_HPP

#include "player.hpp"

class MpvController;
class mpv_opengl_cb_context;

class MpvPlayer : public GLPlayer {
    Q_OBJECT
public:
    explicit MpvPlayer(QWidget* = nullptr);
    ~MpvPlayer();

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;

private:
    MpvController* m_mpv;
    mpv_opengl_cb_context* m_mpv_gl;
private slots:
    static void onUpdate(void*);
    void swapped();
    void maybeUpdate();
};

#endif // MPVPLAYER_HPP
