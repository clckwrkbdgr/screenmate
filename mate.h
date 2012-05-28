#pragma once
#include <QtCore/QPoint>
#include <QtCore/QVector>

namespace MateState {
	enum {FALLING, CAPTURED, ACTIONS, LEFT, RIGHT, STANDING, COUNT};
}

class QPixmap;

class Mate {
public:
	Mate();
	void capture();
	void startFalling();
	bool isCaptured() const;
	void moveToNextFrame();
	int getRandomAction();
	void act();
	QPoint movement();
	void meetGround();
	bool onGround();
	const QPixmap sprite();

	static const QSize & spriteSize();
private:
	QVector<QPixmap> pixmap;
	int actionDelay, currentFrame;
	int state;
};

