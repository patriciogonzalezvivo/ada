
#pragma once

#include "textureStream.h"

#ifdef SUPPORT_LIBAV

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
}

namespace ada {

class TextureStreamAV : public TextureStream {
public:
    TextureStreamAV();
    TextureStreamAV( bool _isDevice );
    virtual ~TextureStreamAV();

    virtual bool    load(const std::string& _filepath, bool _vFlip, TextureFilter _filter = LINEAR, TextureWrap _wrap = REPEAT);
    virtual void    restart();
    virtual void    setSpeed( float _speed );

    virtual double  getFPS();
    virtual float   getTime() const { return (m_device)? 0.0 : m_time; }
    virtual float   getDuration();

    virtual float   getTotalFrames();
    virtual float   getCurrentFrame() const;

    virtual const GLuint    getPrevTextureId() const { return m_idPrev; };

    virtual bool    isDevice() { return m_device; }

    virtual bool    update();
    virtual void    clear();

private:
    int64_t         dts_to_frame_number(int64_t dts);
    double          dts_to_sec(int64_t dts);

    double          currentFramePts();
    bool            decodeFrame();
    bool            newFrame();

    AVFormatContext *av_format_ctx;
    AVCodecContext  *av_codec_ctx;
    AVCodec         *av_decoder;
    AVFrame         *av_frame;
    AVPacket        *av_packet;
    AVRational      time_base;
    SwsContext      *conv_ctx;
    uint8_t         *frame_data;

    double          m_fps;
    double          m_startTime;
    double          m_duration;
    double          m_time;
    double          m_waitFrom;
    double          m_waitUntil;
    double          m_speed;

    long            m_totalFrames;
    long            m_currentFrame;

    int             m_streamId;
    GLuint          m_idPrev;
    bool            m_device;
};

}

#endif
