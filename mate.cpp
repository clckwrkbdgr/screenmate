#include <QtDebug>
#include <QtGui/QPixmap>
#include <QtCore/QMap>

#include "mate.h"

namespace { // Constants.
QMap<int, int> getSpriteForStateMap()
{
	QMap<int, int> map;
	map[MateState::STANDING] = 0;
	map[MateState::LEFT    ] = 1;
	map[MateState::RIGHT   ] = 2;
	map[MateState::FALLING ] = 3;
	map[MateState::CAPTURED] = 4;
	return map;
}

const int FULL_ANIMATION_TIME = 20;
const int ACTION_DELAY = 100;
const QSize SPRITE_SIZE(32, 32);
}

const QSize & Mate::spriteSize()
{
	return SPRITE_SIZE;
}

Mate::Mate()
	: actionDelay(0), currentFrame(0), state(MateState::FALLING)
{
	QPixmap image;
	image.load(":/sprites/sprite");

	int spriteCount = image.width() / spriteSize().width();
	pixmap.resize(spriteCount);
	for(int i = 0; i < spriteCount; ++i) {
		pixmap[i] = image.copy(
				i * spriteSize().width(), 0,
				spriteSize().width(), image.height()
				);
	}
}

const QPixmap Mate::sprite()
{
	static QMap<int, int> spriteForState = getSpriteForStateMap();

	QPixmap & frameStrip = pixmap[spriteForState[state]];
	int frameSpan = FULL_ANIMATION_TIME * spriteSize().height() / frameStrip.height();
	int frame = currentFrame / frameSpan;
	return frameStrip.copy(
			0, frame * spriteSize().height(),
			spriteSize().width(), spriteSize().height()
			);
}

void Mate::capture()
{
	state = MateState::CAPTURED;
}

void Mate::startFalling()
{
	state = MateState::FALLING;
}

bool Mate::isCaptured() const
{
	return state == MateState::CAPTURED;
}

void Mate::moveToNextFrame()
{
	++currentFrame;
	if(currentFrame >= FULL_ANIMATION_TIME) {
		currentFrame = 0;
	}
}

int Mate::getRandomAction()
{
	actionDelay = ACTION_DELAY;
	return MateState::ACTIONS + 1 + qrand() % (MateState::COUNT - MateState::ACTIONS - 1);
}

void Mate::act()
{
	if(onGround()) {
		actionDelay--;
		if(actionDelay <= 0) {
			state = getRandomAction();
		}
	}
}

QPoint Mate::movement()
{
	switch(state) {
		case MateState::FALLING: return QPoint( 0, 1);
		case MateState::LEFT   : return QPoint(-1, 0);
		case MateState::RIGHT  : return QPoint( 1, 0);
		default: break;
	}
	return QPoint();
}

void Mate::meetGround()
{
	if(state == MateState::FALLING) {
		state = getRandomAction();
	}
}

bool Mate::onGround()
{
	return (state > MateState::ACTIONS);
}

