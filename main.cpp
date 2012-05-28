#include <QtGui/QApplication>
#include <QtCore/QTime>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(true);

	qsrand(QTime::currentTime().msec() + QApplication::applicationPid());

	MainWindow w;
	w.setWindowFlags(Qt::FramelessWindowHint | Qt::SplashScreen |
					 Qt::WindowStaysOnTopHint);
	w.show();

	return a.exec();
}
