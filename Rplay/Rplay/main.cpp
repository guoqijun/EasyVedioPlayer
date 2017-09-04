#include "Rplay.h"
#include <QtWidgets/QApplication>
#include "Rxxffmpeg.h"
#include <QAudioOutput>
int main(int argc, char *argv[])
{
	QAudioOutput *out;
	QAudioFormat fmt;
	fmt.setSampleRate(48000);//48000  采集 one second越多越细腻声音
	fmt.setSampleSize(16);//样本大小
	fmt.setChannelCount(2);
	fmt.setCodec("audio/pcm");
	out = new QAudioOutput(fmt);
	QIODevice *ad = out->start();

	fmt.setByteOrder(QAudioFormat::LittleEndian);//表示音频数据的次序
	fmt.setSampleType(QAudioFormat::UnSignedInt);
	QApplication a(argc, argv);
	Rplay w;
	w.show();
	return a.exec();
}
