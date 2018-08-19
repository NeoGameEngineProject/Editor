#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>

#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    
	QFile file(":/dark.qss");
	file.open(QFile::ReadOnly | QFile::Text);
	QTextStream stream(&file);
	a.setStyleSheet(stream.readAll());

	
	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	format.setVersion(4, 5);
	format.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);
    
	MainWindow w;
	w.show();
	
	return a.exec();
}
