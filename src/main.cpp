#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QIcon>

#include <ThreadPool.h>

#include <JsonScene.h>
#include <AssimpScene.h>
#include <BinaryScene.h>

int main(int argc, char *argv[])
{
	Neo::ThreadPool::start();
	
	Neo::BinaryScene binLoader;
	Neo::JsonScene jsonLoader;
	Neo::AssimpScene assimpLoader;

	Neo::LevelLoader::registerLoader(&binLoader);
	Neo::LevelLoader::registerLoader(&jsonLoader);
	Neo::LevelLoader::registerLoader(&assimpLoader);

	//QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
	QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
	QApplication a(argc, argv);
	
	QDir::setCurrent(a.applicationDirPath());

#if 0
	QIcon::setThemeName("breeze-dark");

	QFile file(":/dark.qss");
	file.open(QFile::ReadOnly | QFile::Text);
	QTextStream stream(&file);
	a.setStyleSheet(stream.readAll());
#else
	QIcon::setThemeName("breeze");
#endif

	QSurfaceFormat format;
	format.setDepthBufferSize(32);
	format.setStencilBufferSize(8);
	format.setVersion(4, 5);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	// format.setSamples(4);

#ifdef NDEBUG
	// format.setOption(QSurfaceFormat::DebugContext);
#endif

	QSurfaceFormat::setDefaultFormat(format);

	MainWindow w;
	w.show();
	
	try
	{	
		return a.exec();
	}
	catch(std::exception& e)
	{
		LOG_ERROR("Uncaught exception: " << e.what());
		return 1;
	}
}
