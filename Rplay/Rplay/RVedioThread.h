#pragma once
#include <QThread>
class RVedioThread:public QThread
{
public:
	static RVedioThread *Get()
	{
		static RVedioThread vt;
		return &vt;
	}
	void run();
	RVedioThread();
	virtual ~RVedioThread();
};

