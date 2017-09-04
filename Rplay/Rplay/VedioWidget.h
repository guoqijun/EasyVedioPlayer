#pragma once
#include <QtWidgets/qwidget.h>
#include <QOpenGLWidget>
class VedioWidget:public QOpenGLWidget
{
public:
	VedioWidget(QWidget *p = NULL);
	void paintEvent(QPaintEvent *e);
	void timerEvent(QTimerEvent *e);
	virtual ~VedioWidget();
};

