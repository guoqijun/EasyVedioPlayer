#include "VedioWidget.h"
#include <QPainter>
#include "Rxxffmpeg.h"
#include "RVedioThread.h"
VedioWidget::VedioWidget(QWidget *p):QOpenGLWidget(p)
{
	//Rxxffmpeg::Get()->Open("fuck.mp4");
	startTimer(5);
	RVedioThread::Get()->start();
}

void VedioWidget::paintEvent(QPaintEvent * e)
{
	static int w = 0;
	static int h = 0;
	static QImage *image = NULL;
	if (w != width() || h != height())
	{
		if (image)
		{
			delete image->bits();
			delete image;
			image = NULL;
		}
	
	}
	if (image == NULL)
	{
		uchar *buf = new uchar[width() * height() * 4];
		image = new QImage(buf, width(), height(), QImage::Format_ARGB32);
	}
	/*
	AVPacket pkt = Rxxffmpeg::Get()->Read();
	if (pkt.stream_index != Rxxffmpeg::Get()->vedioStream)
	{
		av_packet_unref(&pkt);
		return;
	}
	if (pkt.size == 0)
	{
		av_packet_unref(&pkt);
		return;
	}
	AVFrame *yuv = Rxxffmpeg::Get()->Decode(&pkt);
	printf("pts:%lld\n", pkt.pts);
	av_packet_unref(&pkt);
	if (yuv == NULL)
	{
		return;
	}*/
	Rxxffmpeg::Get()->ToRGB( (char*)image->bits(), width(), height());
	
	QPainter painter;
	painter.begin(this);
	painter.drawImage(QPoint(0,0), *image);
	painter.end();
}

void VedioWidget::timerEvent(QTimerEvent * e)
{
	//refreash the windows, call repaint
	this->update();
}


VedioWidget::~VedioWidget()
{
	//

}
