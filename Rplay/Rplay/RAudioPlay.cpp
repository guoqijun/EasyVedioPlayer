#include "RAudioPlay.h"
#include <QAudioOutput>
class CRAudioPlay :public RAudioPlay
{
public:
	QAudioOutput *out;
	QIODevice *io = NULL;
	QMutex mutex;
	bool Stop()
	{
		mutex.lock();
		if (out)
		{
			out->stop();
			delete out;
			out = NULL;
			io = NULL;
		}
		mutex.unlock();
		return true;
	}
	bool Start()
	{
		Stop();
		mutex.lock();
		QAudioFormat fmt;
		fmt.setSampleRate(48000);//48000  �ɼ� one secondԽ��Խϸ������
		fmt.setSampleSize(16);//������С
		fmt.setChannelCount(2);
		fmt.setCodec("audio/pcm");
		fmt.setByteOrder(QAudioFormat::LittleEndian);//��ʾ��Ƶ���ݵĴ���
		fmt.setSampleType(QAudioFormat::UnSignedInt);
		out = new QAudioOutput(fmt);
		io = out->start();
		mutex.unlock();
		return true;
	}

	bool Play(bool isPlay)
	{
		mutex.lock();
		if (!out)
		{
			mutex.unlock();
			return false;
		}
		if (isPlay)
		{
			out->resume();
		}
		else
		{
			out->suspend();
		}
		mutex.unlock();
		return true;
	}
	int GetFree()
	{
		mutex.lock();
		if (!out)
		{
			mutex.unlock();
			return 0;
		}
		mutex.unlock();
		return out->bytesFree();
	}
	bool Write(const char *data, int dataSize)
	{
		mutex.lock();
		if (!io)  
		{
			mutex.unlock(); 
			return false;
		}
		//��IO�豸д��Ƶ����
		io->write(data,dataSize);
		mutex.unlock();
		return true;
	}
};


RAudioPlay::RAudioPlay()
{
}



RAudioPlay::~RAudioPlay()
{
}



RAudioPlay * RAudioPlay::Get()
{
	static CRAudioPlay ap;
	return &ap;
}