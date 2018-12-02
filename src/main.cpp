#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>

#include <QFile>
#include <QTextStream>

#include <ThreadPool.h>

int main(int argc, char *argv[])
{
	Neo::ThreadPool::start();
	
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
	QApplication a(argc, argv);
    
	/*QFile file(":/dark.qss");
	file.open(QFile::ReadOnly | QFile::Text);
	QTextStream stream(&file);
	a.setStyleSheet(stream.readAll());*/

	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	format.setVersion(4, 5);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	format.setSamples(4);

#ifdef NDEBUG
	// format.setOption(QSurfaceFormat::DebugContext);
#endif

	QSurfaceFormat::setDefaultFormat(format);
    
	MainWindow w;
	w.show();
	
	return a.exec();
}
