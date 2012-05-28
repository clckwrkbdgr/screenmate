#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QDesktopWidget>
#include <QtGui/QBitmap>
#include <QtGui/QKeyEvent>
#include <QtDebug>

#include "mainwindow.h"

const int TIMER_DELAY = 10;
const double PORTAL_PROB = 0.0003;

bool getProb(double prob)
{
	return qrand() / double(RAND_MAX) < prob;
}

QPoint getRandomPos(const QRect & rect, const QSize & spriteSize)
{
	return QPoint(
			qrand() % (rect.right() - rect.left() - spriteSize.width() ) + rect.left(),
			qrand() % (rect.top() - rect.bottom() - spriteSize.height()) + rect.top()
			);
}

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
	screenRect = QApplication::desktop()->availableGeometry();
	setAutoFillBackground(false);
	setAttribute(Qt::WA_TranslucentBackground, true);
	//setAttribute(Qt::WA_NoSystemBackground, true);

	setGeometry(QRect(getRandomPos(screenRect, Mate::spriteSize()), Mate::spriteSize()));

	timerId = startTimer(TIMER_DELAY);
}

MainWindow::~MainWindow()
{
	killTimer(timerId);
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

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		mate.capture();
		lastPos = event->pos();
	}
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton) {
		mate.startFalling();
	}
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
	if(mate.isCaptured() && event->buttons().testFlag(Qt::LeftButton)) {
		move(pos() + event->pos() - lastPos);
	}
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

	if(getProb(PORTAL_PROB)) {
		move(getRandomPos(screenRect, Mate::spriteSize()));
		mate.startFalling();
	}

	update();
}

