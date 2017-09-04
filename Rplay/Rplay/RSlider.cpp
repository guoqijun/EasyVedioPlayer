#include "RSlider.h"
#include <QMouseEvent>
//传给基类的构造函数
RSlider::RSlider(QWidget  *p):QSlider(p)
{

}


RSlider::~RSlider()
{
}

void RSlider::mousePressEvent(QMouseEvent * e)
{
	int  value = ((float)e->pos().x() / (float)this->width()) * (this->maximum() + 1);
	this->setValue(value);
	QSlider::mousePressEvent(e);
}
