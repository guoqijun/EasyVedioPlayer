#include "Rxxffmpeg.h"

#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"swresample.lib")
static double r2d(AVRational r)
{
	return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}
int Rxxffmpeg::Open(const char * path)
{
	Close();
	mutex.lock();
	int ret = avformat_open_input(&ic, path, NULL, NULL);
	if (ret != 0)
	{
		mutex.unlock();
		av_strerror(ret, errorBuf, sizeof(errorBuf));
		printf("open error:%s ,reason:%s\n", path, errorBuf);
		return false;
	}
	totalMs = (ic->duration / AV_TIME_BASE)*1000;
	
	for (int i = 0; i<ic->nb_streams; i++)
	{
		AVCodecContext *enc = ic->streams[i]->codec;

		if (enc->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			vedioStream = i;
			fps = r2d(ic->streams[i]->avg_frame_rate);
			AVCodec *codec = avcodec_find_decoder(enc->codec_id);
			if (codec == NULL)
			{
				mutex.unlock();
				printf("can not find vedio decoder\n");
				getchar();
				return 0;
			}
			int err = avcodec_open2(enc, codec, NULL);
			if (err != 0)
			{
				char buf[1024] = { 0 };
				mutex.unlock();
				av_strerror(err, buf, sizeof(buf));
				printf("open error:%s ,reason:%s\n", path, buf);
				getchar();
				return 0;
			}
			//printf("open vedio decoder success !vedioStream:%d\n", vedioStream);
		}
		else if (enc->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audioStream = i;
			//AVCodecContext *enc = ic->streams[i]->codec;
			//find audio decoder
			AVCodec *codec = avcodec_find_decoder(enc->codec_id);
			if (avcodec_open2(enc, codec, NULL) < 0)
			{
				mutex.unlock();
				return false;
			}
			this->sampleRate = enc->sample_rate;
			this->channelCount = enc->channels;
			switch (enc->sample_fmt)
			{
			case AV_SAMPLE_FMT_S16:
				this->sampleSize = 16;
				break;
			case AV_SAMPLE_FMT_S32:
				this->sampleSize = 32;
				break;
			default:
				break;
			}
			printf("sampleRate:%d, channelCount:%d \n", this->sampleRate, this->channelCount);
		}

	}
	mutex.unlock();
	return totalMs;
}

void Rxxffmpeg::Close()
{
	mutex.lock();
	if (ic) avformat_close_input(&ic);
	if (yuv) av_frame_free(&yuv);
	if (cCtx)
	{
		sws_freeContext(cCtx);
		cCtx = NULL;
	}
	if (aCtx)
	{
		swr_free(&aCtx);
		aCtx = NULL;
	}
	mutex.unlock();
}

AVPacket Rxxffmpeg::Read()
{
	AVPacket pkt;
	memset(&pkt,0,sizeof(AVPacket));
	mutex.lock();
	if (ic == NULL)
	{
		mutex.unlock();
		return pkt;
	}
	int ret = av_read_frame(ic, &pkt);
	if (ret != 0)
	{
		mutex.unlock();
		av_packet_unref(&pkt);
		av_strerror(ret, errorBuf, sizeof(errorBuf));
		printf("read error !!,reason:%s\n", errorBuf);
		return pkt;
	}
	mutex.unlock();
	return pkt;
}

int Rxxffmpeg::GetPts(const AVPacket *pkt)
{
	mutex.lock();
	if (!ic)
	{
		mutex.unlock();
		return -1;
	}
	int pts = 1000 * (pkt->pts * r2d(ic->streams[pkt->stream_index]->time_base));
	mutex.unlock();
	return pts;
}

//return pts
int  Rxxffmpeg::Decode(const AVPacket *pkt)
{
	mutex.lock();
	if (&ic == NULL)
	{
		mutex.unlock();
		return NULL;
	}
	if (yuv == NULL)
	{
		yuv = av_frame_alloc();
	}
	if (pcm == NULL)
	{
		pcm = av_frame_alloc();
	}
	AVFrame *frame = yuv;
	if (pkt->stream_index == audioStream)
	{
		frame = pcm;
	}
	int ret = avcodec_send_packet(ic->streams[pkt->stream_index]->codec, pkt);
	if (ret != 0)
	{
		mutex.unlock();
		av_strerror(ret, errorBuf, sizeof(errorBuf));
		return NULL;
	}
	//写到PCM或者YUV里面
	ret = avcodec_receive_frame(ic->streams[pkt->stream_index]->codec, frame);
	if (ret != 0)
	{
		mutex.unlock();
		av_strerror(ret, errorBuf, sizeof(errorBuf));
		return NULL;
	}
	mutex.unlock();
	int p = 1000*(frame->pts * r2d(ic->streams[pkt->stream_index]->time_base));
	if (pkt->stream_index == audioStream)
		this->pts = p;
	return p;
}

bool Rxxffmpeg::ToRGB(char * out, int outweighr, int outheight)
{
	mutex.lock();
	if (!ic || !yuv)
	{
		mutex.unlock();
		return false;
	}
	//转码器，视频源[YUVXXX]转成RGBA
	AVCodecContext *vedioCtx = ic->streams[this->vedioStream]->codec;
	if (!vedioCtx)
	{
		mutex.unlock();
		return false;
	}
	cCtx = sws_getCachedContext(cCtx, vedioCtx->width, vedioCtx->height, vedioCtx->pix_fmt,
		outweighr, outheight, AV_PIX_FMT_BGRA, SWS_BICUBIC, NULL, NULL, NULL);
	if (cCtx == NULL)
	{
		mutex.unlock();
		printf("sws_getCachedContext fail\n");
		return false;;
	}
	//缩放参数
	uint8_t *data[AV_NUM_DATA_POINTERS] = { 0 };
	data[0] = (uint8_t *)out;
	int lineSize[AV_NUM_DATA_POINTERS] = { 0 };
	lineSize[0] = outweighr * 4;
	//缩放
	int h = sws_scale(cCtx, yuv->data, yuv->linesize, 0, yuv->height, data, lineSize);
	mutex.unlock();

	return true;
}

int Rxxffmpeg::ToPCM(char * out)
{
	mutex.lock();
	if (!ic || !pcm || !out)
	{
		return false;
		mutex.unlock();
	}
	AVCodecContext *ctx = ic->streams[audioStream]->codec;
	if (aCtx == NULL)
	{
		aCtx = swr_alloc();
		swr_alloc_set_opts(aCtx, ctx->channel_layout,AV_SAMPLE_FMT_S16,
			ctx->sample_rate,ctx->channels,ctx->sample_fmt, ctx->sample_rate,0,0);
		swr_init(aCtx);
	}
	uint8_t *data[1];
	data[0] = (uint8_t *)out;
	int len = swr_convert(aCtx, data ,10000,
		(const uint8_t **)pcm->data,pcm->nb_samples);
	if (len < 0)
	{
		mutex.unlock();
		return false;
	}
	int outsize = av_samples_get_buffer_size(NULL,ctx->channels,pcm->nb_samples,AV_SAMPLE_FMT_S16
	,0);

	mutex.unlock();
	return outsize;
}

bool Rxxffmpeg::Seek(float pos)
{
	mutex.lock();
	if (!ic)
	{
		mutex.unlock();
		return false;
	}
	//seek没有实现找到I帧解码的功能
	int64_t stamp = 0;
	stamp = pos * ic->streams[vedioStream]->duration;
	//seek backward  and seek IDR  frame
	int re = av_seek_frame(ic,vedioStream,stamp,AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_FRAME);
	//codec   句柄，把缓存帧清空，也就是参考帧队列
	avcodec_flush_buffers(ic->streams[vedioStream]->codec);
	pts = 1000 * (stamp * r2d(ic->streams[vedioStream]->time_base));
	if (re <= 0)
	{
		mutex.unlock();
		return false;
	}
	
	mutex.unlock();
	return true;
}

std::string Rxxffmpeg::GetError()
{
	mutex.lock();
	std::string re = this->errorBuf;//拷贝过去
	mutex.unlock();

	return re;
}

Rxxffmpeg::Rxxffmpeg()
{
	errorBuf[0] = 0;
	av_register_all();
}


Rxxffmpeg::~Rxxffmpeg()
{
}
