#pragma once
#include <qslider.h>
class RSlider:public QSlider
{
	//QT special simbol
	Q_OBJECT
public:
	RSlider(QWidget *p = NULL);
	virtual ~RSlider();
	void mousePressEvent(QMouseEvent *e);
};

