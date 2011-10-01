#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>
#include <QtDebug>
#include <windows.h>

#include "groundengine.h"

QRect GroundEngine::getWndRectFromPoint(const QPoint &pos)
{
	POINT p;
	p.x=pos.x();
	p.y=pos.y();
	HWND hwnd=WindowFromPoint(p);

	//get rect
	RECT winRect;
	if(GetWindowRect(hwnd,&winRect))
		return QRect(QPoint(winRect.left,winRect.top),
							QPoint(winRect.right,winRect.bottom));
	else
		return QRect();
}

bool GroundEngine::canStand(const QRect &ground,const QRect &rect)
{
	if(ground.top()>0) //at it's top is not on the very top of screen - not the roof, ..
	{
		//..and caption can be the ground
		if(rect.right()>=ground.left() && rect.left()<=ground.right() &&
															qAbs(rect.bottom()-ground.top())==1)
			return true;
	}

	return false;
}

bool GroundEngine::hasGround(const QRect &rect)
{
	QPoint leftBottom=rect.bottomLeft()+QPoint(0,1);
	QPoint rightBottom=rect.bottomRight()+QPoint(0,1);

	QList<QRect> grounds;
	grounds<<getWndRectFromPoint(leftBottom)<<getWndRectFromPoint(rightBottom)<<
			QRect(0, QApplication::desktop()->availableGeometry().height() - 1,
										QApplication::desktop()->availableGeometry().width(), 10);

	foreach(QRect ground,grounds)
		if(canStand(ground,rect))
			return true;

	return false;
}

