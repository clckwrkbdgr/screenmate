#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QWidget>

class QTimer;
class QPaintEvent;
class QKeyEvent;
class QMouseEvent;
class QPixmap;

class MainWindow : public QWidget
{
    Q_OBJECT

	int timerId;

	QPixmap sprite;

	enum Action {Standing=0,MovingLeft=1,MovingRight=2,
				 ActionCount=3} action;
	enum State {Captured=0,Falling=1,Acting=2} state;

	int actionDelay, currentFrame;
	QPoint lastPos;
	
	int screenWidth,screenHeight;
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
