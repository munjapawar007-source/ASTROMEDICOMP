#include "E_video.h"
#include "E_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

struct E_video
{
    AVFormatContext* formatCtx;
    AVCodecContext*  codecCtx;
    SwsContext*      swsCtx;
    AVFrame*         decodedFrame;
    AVFrame*         rgbFrame;
    AVPacket*        packet;

    int    streamIndex;
    int    width;
    int    height;
    int    loop;
    int    finished;

    double frameInterval;
    double lastFrameTime;
    double startElapsed;
    int    currentFrame;

    GLuint texture;
};


static void uploadFrame(E_video* v)
{
    if (!v->rgbFrame->data[0]) return;
    glBindTexture(GL_TEXTURE_2D, v->texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, v->width, v->height,
                    GL_RGB, GL_UNSIGNED_BYTE, v->rgbFrame->data[0]);
    glBindTexture(GL_TEXTURE_2D, 0);
}

static int decodeNextFrame(E_video* v)
{
    int result;

    while ((result = av_read_frame(v->formatCtx, v->packet)) >= 0)
    {
        if (v->packet->stream_index != v->streamIndex)
        {
            av_packet_unref(v->packet);
            continue;
        }

        result = avcodec_send_packet(v->codecCtx, v->packet);
        av_packet_unref(v->packet);
        if (result < 0) return 0;

        result = avcodec_receive_frame(v->codecCtx, v->decodedFrame);
        if (result == 0)
        {
            sws_scale(v->swsCtx,
                      v->decodedFrame->data, v->decodedFrame->linesize, 0, v->height,
                      v->rgbFrame->data,     v->rgbFrame->linesize);
            av_frame_unref(v->decodedFrame);
            uploadFrame(v);
            return 1;
        }
        if (result != AVERROR(EAGAIN)) return 0;
    }

    // EOF  drain buffered frames from the decoder
    avcodec_send_packet(v->codecCtx, NULL);
    while (avcodec_receive_frame(v->codecCtx, v->decodedFrame) == 0)
    {
        sws_scale(v->swsCtx,
                  v->decodedFrame->data, v->decodedFrame->linesize, 0, v->height,
                  v->rgbFrame->data,     v->rgbFrame->linesize);
        av_frame_unref(v->decodedFrame);
        uploadFrame(v);
        return 1;
    }

    // Truly at end  loop without recursion
    if (v->loop)
    {
        avcodec_flush_buffers(v->codecCtx);
        if (av_seek_frame(v->formatCtx, v->streamIndex, 0, AVSEEK_FLAG_BACKWARD) < 0)
            av_seek_frame(v->formatCtx, -1, 0, AVSEEK_FLAG_BACKWARD);
        // Let the next E_video_update call decode the first frame after seek
        return 0;
    }

    v->finished = 1;
    return 0;
}

// Video api
E_video* E_video_load(const char* path, int loop)
{
    E_video* v = (E_video*)calloc(1, sizeof(E_video));
    if (!v) return NULL;

    v->loop         = loop;
    v->lastFrameTime = -999.0;
    v->startElapsed  = -1.0;
    v->currentFrame  = 0;

    if (avformat_open_input(&v->formatCtx, path, NULL, NULL) < 0)
    {
        fprintf(stderr, "[E_video] Cannot open: %s\n", path);
        free(v); return NULL;
    }

    if (avformat_find_stream_info(v->formatCtx, NULL) < 0)
    {
        fprintf(stderr, "[E_video] No stream info: %s\n", path);
        avformat_close_input(&v->formatCtx);
        free(v); return NULL;
    }

    int si = av_find_best_stream(v->formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (si < 0)
    {
        fprintf(stderr, "[E_video] No video stream: %s\n", path);
        avformat_close_input(&v->formatCtx);
        free(v); return NULL;
    }
    v->streamIndex = si;

    AVStream* stream = v->formatCtx->streams[si];
    double fps = av_q2d(stream->avg_frame_rate);
    if (fps <= 0.0) fps = av_q2d(stream->r_frame_rate);
    v->frameInterval = (fps > 0.0) ? 1.0 / fps : 1.0 / 30.0;

    const AVCodec* decoder = avcodec_find_decoder(stream->codecpar->codec_id);
    v->codecCtx = avcodec_alloc_context3(decoder);
    avcodec_parameters_to_context(v->codecCtx, stream->codecpar);
    if (avcodec_open2(v->codecCtx, decoder, NULL) < 0)
    {
        fprintf(stderr, "[E_video] Cannot open codec: %s\n", path);
        avformat_close_input(&v->formatCtx);
        free(v); return NULL;
    }

    v->width  = v->codecCtx->width;
    v->height = v->codecCtx->height;

    v->swsCtx = sws_getContext(v->width, v->height, v->codecCtx->pix_fmt,
                               v->width, v->height, AV_PIX_FMT_RGB24,
                               SWS_BILINEAR, NULL, NULL, NULL);

    v->decodedFrame = av_frame_alloc();
    v->rgbFrame     = av_frame_alloc();
    v->packet       = av_packet_alloc();
    if (!v->decodedFrame || !v->rgbFrame || !v->packet)
    {
        fprintf(stderr, "[E_video] Frame/packet alloc failed: %s\n", path);
        E_video_destroy(v); return NULL;
    }
    if (av_image_alloc(v->rgbFrame->data, v->rgbFrame->linesize,
                       v->width, v->height, AV_PIX_FMT_RGB24, 1) < 0)
    {
        fprintf(stderr, "[E_video] RGB buffer alloc failed: %s\n", path);
        E_video_destroy(v); return NULL;
    }

    // Create GL texture
    glGenTextures(1, &v->texture);
    glBindTexture(GL_TEXTURE_2D, v->texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, v->width, v->height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Decode first frame immediately so texture is valid from the start
    decodeNextFrame(v);

    return v;
}

void E_video_update(E_video* v)
{
    if (!v || v->finished) return;

    double now = E_Utils_getElapsedTimeInSeconds();

    if (v->loop)
    {
        // Wall-clock interval mode: advance one frame at a time, doesn't track elapsed time
        if (now - v->lastFrameTime < v->frameInterval) return;
        v->lastFrameTime = now;
        decodeNextFrame(v);
    }
    else
    {
        // Elapsed-time mode: lazy-init on first call, then seek to correct frame
        if (v->startElapsed < 0.0)
        {
            v->startElapsed = now;
            v->currentFrame = 0;
            decodeNextFrame(v);
            return;
        }
        int targetFrame = (int)((now - v->startElapsed) / v->frameInterval);
        while (v->currentFrame < targetFrame)
        {
            if (!decodeNextFrame(v)) break;
            v->currentFrame++;
        }
    }
}

GLuint E_video_getTexture(const E_video* v)
{
    return v ? v->texture : 0;
}

int E_video_isFinished(const E_video* v)
{
    return v ? v->finished : 1;
}

void E_video_rewind(E_video* v)
{
    if (!v) return;
    v->finished      = 0;
    v->lastFrameTime = -999.0;
    v->startElapsed  = E_Utils_getElapsedTimeInSeconds();
    v->currentFrame  = 0;
    avcodec_flush_buffers(v->codecCtx);
    av_seek_frame(v->formatCtx, v->streamIndex, 0, AVSEEK_FLAG_BACKWARD);
    decodeNextFrame(v);
}

void E_video_destroy(E_video* v)
{
    if (!v) return;
    if (v->texture) glDeleteTextures(1, &v->texture);
    if (v->rgbFrame) { av_freep(&v->rgbFrame->data[0]); av_frame_free(&v->rgbFrame); }
    if (v->decodedFrame) av_frame_free(&v->decodedFrame);
    if (v->packet)       av_packet_free(&v->packet);
    if (v->swsCtx)       sws_freeContext(v->swsCtx);
    if (v->codecCtx)     avcodec_free_context(&v->codecCtx);
    if (v->formatCtx)    avformat_close_input(&v->formatCtx);
    free(v);
}
