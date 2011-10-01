#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QDesktopWidget>
#include <QtGui/QBitmap>
#include <QtGui/QKeyEvent>
#include <QtDebug>

#include "groundengine.h"
#include "mainwindow.h"

const QSize spriteSize(32,32);

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
	QPixmap p("../usr/shadow.png");
	p.setMask(p.createMaskFromColor(Qt::magenta));
	p.save("../usr/shadow.png");

	qsrand(time(NULL) + QApplication::applicationPid());

	//window
	screenWidth=QApplication::desktop()->availableGeometry().width();
	screenHeight=QApplication::desktop()->availableGeometry().height();
	setAutoFillBackground(false);
	setAttribute(Qt::WA_TranslucentBackground);

	QRect newRect=QRect(QPoint(),spriteSize);
	newRect.moveBottom(0);
	newRect.moveLeft(rand() % (screenWidth-spriteSize.width()));
	setGeometry(newRect);

	//object
	lastPos=QPoint(0,0);
	actionDelay=0;
	currentFrame=0;
	state=Falling;
	action=Standing;
	sprite.load(":/sprites/res");

	//timer
	timerId=startTimer(10);
}

MainWindow::~MainWindow()
{
	killTimer(timerId);
}

void MainWindow::paintEvent(QPaintEvent*)
{
	//get right sprite index in sprite table
	int spriteIndex=0;
	switch(state)
	{
		case Falling: spriteIndex=3; break;
		case Captured: spriteIndex=4; break;
		case Acting: switch(action)
			{
				case Standing: spriteIndex=0; break;
				case MovingRight: spriteIndex=2; break;
				case MovingLeft: spriteIndex=1; break;
				default: break;
			}
			break;
		default: break;
	}

	//drawing
	QPainter painter(this);
	painter.drawPixmap(QPoint(0,0),sprite,
			QRect(spriteIndex * spriteSize.width(),
				  int(currentFrame/10) * spriteSize.height(),
				  spriteSize.width(),
				  spriteSize.height()));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	switch(event->key())
	{
		case Qt::Key_Escape: qApp->quit(); break;
		default: QWidget::keyPressEvent(event);
	}
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	if(event->button()==Qt::LeftButton)
	{
		state=Captured;
		lastPos=event->pos();
	}
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button()==Qt::LeftButton)
		state=Falling;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
	if(state==Captured && event->buttons().testFlag(Qt::LeftButton))
		move(pos()+event->pos()-lastPos);
}

void MainWindow::timerEvent(QTimerEvent*)
{
	//animation
	currentFrame++;
	if(currentFrame>19) currentFrame=0;
	
	//do some actions
	if(state==Acting)
	{
		actionDelay--;
		if(actionDelay<=0) //means it is stand free and can act
		{
			action=Action(qrand()%ActionCount);
			actionDelay=100;
		}
	}

	switch(state)
	{
		case Falling: move(pos()+QPoint(0,1)); break;
		case Acting: switch(action)
			{
				case MovingLeft: move(pos() - QPoint(1,0)); break;
				case MovingRight: move(pos() + QPoint(1,0)); break;
				default: break;
			}
		default: break;
	}

	//checking the earth under the foots
	QRect actualGeometry=geometry();
	actualGeometry.setWidth(actualGeometry.width() / 2); //only two center quarters has value
	actualGeometry.moveCenter(geometry().center());

	if(GroundEngine::hasGround(actualGeometry))
	{
		if(state==Falling)
		{
			state=Acting;
			actionDelay=0;
		}
	}
	else
	{
		if(state==Acting)
			state=Falling;
	}

	//rebirth if should
	if((actualGeometry.right()<=0 || actualGeometry.left()>=screenWidth) || y()>screenHeight)
	{
		move(qrand()%(screenWidth-width()),-height());
		state=Falling;
	}

	//draw
	update();
}

