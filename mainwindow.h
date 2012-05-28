#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QWidget>

class QTimer;
class QPaintEvent;
class QKeyEvent;
class QMouseEvent;
class QPixmap;

namespace MateState {
	enum {FALLING, CAPTURED, ACTIONS, LEFT, RIGHT, STANDING, COUNT};
}

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
private:
	QVector<QPixmap> pixmap;
	int actionDelay, currentFrame;
	int state;
};

class MainWindow : public QWidget
{
    Q_OBJECT

	int timerId;
	Mate mate;
	QPoint lastPos;
	QRect screenRect;
protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void timerEvent(QTimerEvent *event);
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
