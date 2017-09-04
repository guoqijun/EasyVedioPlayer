#pragma once
extern "C" {
#include <libavformat\avformat.h>
#include <libswscale/swscale.h>
#include <libswresample\swresample.h>
}
#include <string>
#include <qmutex.h>

class Rxxffmpeg
{
public:
	static Rxxffmpeg *Get()
	{
		static Rxxffmpeg ff;
		return &ff;
	}
	int Open(const char * path);
	void Close();
	AVPacket Read();


	int GetPts(const AVPacket *pkt);
	int Decode(const AVPacket *pkt);
	bool ToRGB(char *out ,int outweighr, int outheight);
	int ToPCM(char *out);
	bool Seek(float pos);
	std::string GetError();
	//使用string而不是char *返回类型是为了避免多通道解码时，返回的地址被踩。用string不会被踩
	virtual ~Rxxffmpeg();
	int totalMs = 0;
	int fps = 0;
	int pts = 0;
	int vedioStream;
	int audioStream = 1;
	//设置默认值，openfile的时候读取文件属性
	int sampleRate = 48000;
	int sampleSize = 16;
	int channelCount = 2;

	bool isPlay = false;
protected:
	char errorBuf[1024];

	AVFormatContext *ic = NULL;
	QMutex mutex;
	AVFrame *yuv = NULL;
	AVFrame *pcm = NULL;
	SwsContext *cCtx = NULL;
	SwrContext *aCtx = NULL;
	Rxxffmpeg();
};

