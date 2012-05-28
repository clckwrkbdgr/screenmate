#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QDesktopWidget>
#include <QtGui/QBitmap>
#include <QtGui/QKeyEvent>
#include <QtDebug>

#include "mainwindow.h"

const int TIMER_DELAY = 10;
const QSize SPRITE_SIZE(32,32);

QPoint getRandomPos(const QRect & rect, const QSize & spriteSize)
{
	return QPoint(
			rand() % (rect.right() - rect.left() - spriteSize.width() ) + rect.left(),
			rand() % (rect.top() - rect.bottom() - spriteSize.height()) + rect.top()
			);
}

Mate::Mate()
	: actionDelay(0), currentFrame(0), state(MateState::FALLING)
{
	QPixmap image;
	image.load(":/sprites/sprite");

	int spriteCount = image.width() / SPRITE_SIZE.width();
	pixmap.resize(spriteCount);
	for(int i = 0; i < spriteCount; ++i) {
		pixmap[i] = image.copy(
				i * SPRITE_SIZE.width(), 0,
				SPRITE_SIZE.width(), image.height()
				);
	}
}

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
	screenRect = QApplication::desktop()->availableGeometry();
	setAutoFillBackground(false);
	setAttribute(Qt::WA_TranslucentBackground);
	//setAttribute(Qt::WA_NoSystemBackground, true );

	setGeometry(QRect(getRandomPos(screenRect, SPRITE_SIZE), SPRITE_SIZE));

	timerId = startTimer(TIMER_DELAY);
}

MainWindow::~MainWindow()
{
	killTimer(timerId);
}

const QPixmap Mate::sprite()
{
	QMap<int, int> spriteForState;
	spriteForState[MateState::STANDING] = 0;
	spriteForState[MateState::LEFT    ] = 1;
	spriteForState[MateState::RIGHT   ] = 2;
	spriteForState[MateState::FALLING ] = 3;
	spriteForState[MateState::CAPTURED] = 4;

	QPixmap & frameStrip = pixmap[spriteForState[state]];
	return frameStrip.copy(
			0, int(currentFrame / 10) * SPRITE_SIZE.height(),
			SPRITE_SIZE.width(), SPRITE_SIZE.height()
			);
}

void MainWindow::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.fillRect(rect(), Qt::transparent);
	painter.drawPixmap(QPoint(0, 0), mate.sprite());
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	switch(event->key())
	{
		case Qt::Key_Escape: qApp->quit(); break;
		default: QWidget::keyPressEvent(event);
	}
}

void Mate::capture()
{
	state = MateState::CAPTURED;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		mate.capture();
		lastPos = event->pos();
	}
}

void Mate::startFalling()
{
	state = MateState::FALLING;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton) {
		mate.startFalling();
	}
}

bool Mate::isCaptured() const
{
	return state == MateState::CAPTURED;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
	if(mate.isCaptured() && event->buttons().testFlag(Qt::LeftButton)) {
		move(pos() + event->pos() - lastPos);
	}
}

void Mate::moveToNextFrame()
{
	++currentFrame;
	if(currentFrame > 19) {
		currentFrame = 0;
	}
}

int Mate::getRandomAction()
{
	return MateState::ACTIONS + 1 + qrand() % (MateState::COUNT - MateState::ACTIONS - 1);
}

void Mate::act()
{
	if(onGround())
	{
		actionDelay--;
		if(actionDelay <= 0) // Means it stands free and can act.
		{
			state = getRandomAction();
			actionDelay = 100;
		}
	}
}

QPoint Mate::movement()
{
	switch(state)
	{
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
		actionDelay = 0;
	}
}

bool Mate::onGround()
{
	return (state > MateState::ACTIONS);
}

void MainWindow::timerEvent(QTimerEvent*)
{
	mate.moveToNextFrame();
	mate.act();
	move(pos() + mate.movement());

	if(geometry().bottom() > screenRect.bottom()) {
		QRect newGeometry = geometry();
		newGeometry.moveBottom(screenRect.bottom());
		setGeometry(newGeometry);

		mate.meetGround();
	} else if(!mate.isCaptured() && geometry().bottom() < screenRect.bottom()) {
		mate.startFalling();
	}

	if(!(rand() % 1000)) {
		move(getRandomPos(screenRect, SPRITE_SIZE));
		mate.startFalling();
	}

	update();
}

