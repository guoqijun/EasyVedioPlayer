#include "RVedioThread.h"
#include "Rxxffmpeg.h"
#include "RAudioPlay.h"
#include <list>
using namespace std;
bool IsExit = false;
static list<AVPacket> vedios;
static int apts = -1;
void RVedioThread::run()
{
	char out[10000] = { 0 };
	while (!IsExit)
	{
		if (Rxxffmpeg::Get()->isPlay)
		{
			msleep(10);
			continue;
		}
		while (vedios.size()>0)
		{
			AVPacket pack = vedios.front();
			int pts = Rxxffmpeg::Get()->GetPts(&pack);
			if (pts > apts)
			{
				break;
			}
			Rxxffmpeg::Get()->Decode(&pack);
			av_packet_unref(&pack);
			vedios.pop_front();
		}
		int free = RAudioPlay::Get()->GetFree();
		if (free <= 10000)
		{
			msleep(1);
			continue;
		}

		AVPacket pkt = Rxxffmpeg::Get()->Read();
		if (pkt.size <= 0)
		{
			msleep(10);
			continue;
		}
		if (pkt.stream_index == Rxxffmpeg::Get()->audioStream)
		{
			apts = Rxxffmpeg::Get()->Decode(&pkt);
			av_packet_unref(&pkt);
			int len = Rxxffmpeg::Get()->ToPCM(out);
			RAudioPlay::Get()->Write(out,len);
			continue;
		}
		if (pkt.stream_index != Rxxffmpeg::Get()->vedioStream)
		{
			av_packet_unref(&pkt);
			continue;
		}
		//Rxxffmpeg::Get()->Decode(&pkt);
		//av_packet_unref(&pkt);
		//if (Rxxffmpeg::Get()->fps > 0)
		vedios.push_back(pkt);
		//msleep(1000 / Rxxffmpeg::Get()->fps);
	}
}

RVedioThread::RVedioThread()
{
}


RVedioThread::~RVedioThread()
{
}
