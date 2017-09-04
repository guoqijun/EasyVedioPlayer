#include "Rplay.h"
#include <QtWidgets/QApplication>
#include "Rxxffmpeg.h"
#include <QAudioOutput>
int main(int argc, char *argv[])
{
	QAudioOutput *out;
	QAudioFormat fmt;
	fmt.setSampleRate(48000);//48000  �ɼ� one secondԽ��Խϸ������
	fmt.setSampleSize(16);//������С
	fmt.setChannelCount(2);
	fmt.setCodec("audio/pcm");
	out = new QAudioOutput(fmt);
	QIODevice *ad = out->start();

	fmt.setByteOrder(QAudioFormat::LittleEndian);//��ʾ��Ƶ���ݵĴ���
	fmt.setSampleType(QAudioFormat::UnSignedInt);
	QApplication a(argc, argv);
	Rplay w;
	w.show();
	return a.exec();
}
