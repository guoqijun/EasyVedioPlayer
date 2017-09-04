#include "RSlider.h"
#include <QMouseEvent>
//��������Ĺ��캯��
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
