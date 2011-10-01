#ifndef GROUNDENGINE_H
#define GROUNDENGINE_H

#include <QtCore/QRect>

class GroundEngine
{
	static QRect getWndRectFromPoint(const QPoint &pos);
	static bool canStand(const QRect &ground,const QRect &rect);
public:
	static bool hasGround(const QRect &rect);
};

#endif // GROUNDENGINE_H
