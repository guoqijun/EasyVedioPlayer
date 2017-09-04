#include "Rplay.h"
#include "QFileDialog.h"
#include "Rxxffmpeg.h"
#include "QMessageBox.h"
#include "RAudioPlay.h"
static bool isPress = true;
static bool isPlay = true;
#define PAUSE "QPushButton:hover\
{border-image: url(:/RRplay/pause_normal.png);}"

#define START "QPushButton:!hover\
{border-image: url(:/RRplay/pause_hot.png);}"
Rplay::Rplay(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//UI线程定时器//
	startTimer(40);
}

void Rplay::openFile(QString name)
{
	if (name.isEmpty())
	{
		return;
	}
	this->setWindowTitle(name);
	int totalMs = Rxxffmpeg::Get()->Open(name.toLocal8Bit());
	if (totalMs <= 0)
	{
		QMessageBox::information(this, "err", "file open file");
		return;
	}
	RAudioPlay::Get()->sampleRate = Rxxffmpeg::Get()->sampleRate;
	RAudioPlay::Get()->channelCount = Rxxffmpeg::Get()->channelCount;
	RAudioPlay::Get()->sampleSize = 16;
	RAudioPlay::Get()->Start();
	char buf[1024] = { 0 };
	sprintf(buf, "%03d.%02d", totalMs / (1000 * 60), (totalMs / 1000) % 60);
	ui.totaltime->setText(buf);
	isPlay = true;
}


void Rplay::open()
{
	QString name = QFileDialog::getOpenFileName(
		this, QString::fromLocal8Bit("select vedio file"));
	openFile(name);
}	


void Rplay::play()
{
	isPlay = !isPlay;
	if (isPlay == true)
	{
		ui.playButton->setStyleSheet(PAUSE);
	}
	else
	{
		ui.playButton->setStyleSheet(START);
	}
	Rxxffmpeg::Get()->isPlay = !isPlay;

}

void Rplay::timerEvent(QTimerEvent * e)
{
	int time = Rxxffmpeg::Get()->pts;
	char buf[1024] = { 0 };
	if (time > 0)
	{
		printf("(%d)\n", time);
	}
	sprintf(buf, "%03d.%02d", time / (1000 * 60), (time / 1000) % 60);
	
	ui.playtime->setText(buf);
	if (Rxxffmpeg::Get()->totalMs > 0)
	{
		float rate = (float)Rxxffmpeg::Get()->pts / (float)Rxxffmpeg::Get()->totalMs;
		if (isPress == true)
		{
			ui.playline->setValue(rate * 1000);
		}
			
	}
	
}
void Rplay::resizeEvent(QResizeEvent * e)
{
	ui.openGLWidget->resize(size());
	ui.playButton->move(this->width() /2 + 40, this->height() - 100);
	ui.openButton->move(this->width() / 2 - 40, this->height() - 110);
	ui.playline->move(50, this->height() - 30);
	ui.playline->resize(this->width()-50, this->height());
}

void Rplay::sliderPress()
{
	isPress = false;
}
void Rplay::sliderRelease()
{
	float pos = (float)ui.playline->value() / (float)(ui.playline->maximum() + 1);
	Rxxffmpeg::Get()->Seek(pos);
	isPress = true;
}

Rplay::~Rplay()
{
}