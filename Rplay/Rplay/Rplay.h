#pragma once

#include <QtWidgets/QWidget>
#include "ui_Rplay.h"

class Rplay : public QWidget
{
	Q_OBJECT

public:
	Rplay(QWidget *parent = Q_NULLPTR);
	~Rplay();
	void timerEvent(QTimerEvent *e);
	void resizeEvent(QResizeEvent *e);
	void openFile(QString name);
public slots:
	void open();
	void sliderPress();
	void sliderRelease();
	void play();
private:
	Ui::RplayClass ui;
};
