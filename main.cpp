#include <QtGui/QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(true);

	MainWindow w;
	w.setWindowFlags(Qt::FramelessWindowHint | Qt::SplashScreen |
					 Qt::WindowStaysOnTopHint);
	w.show();

	return a.exec();
}
