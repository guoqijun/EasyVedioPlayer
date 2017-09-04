#pragma once

#include <qmutex.h>

class RAudioPlay
{
public:
	static RAudioPlay *Get();
	virtual bool Start() = 0;
	virtual bool Stop() = 0;
	virtual bool Play(bool isPlay) = 0;
	virtual bool Write(const char *data , int dataSize) = 0;
	virtual int GetFree() = 0;
	int sampleRate = 48000;
	int sampleSize = 16;
	int channelCount = 2;
	virtual ~RAudioPlay();
protected:
	RAudioPlay();
};

