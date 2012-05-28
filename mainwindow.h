#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QWidget>

#include "mate.h"

class QTimer;
class QPaintEvent;
class QKeyEvent;
class QMouseEvent;
class QPixmap;

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
