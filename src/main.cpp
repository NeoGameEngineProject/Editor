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
#include <glTFScene.h>

int main(int argc, char *argv[])
{
	Neo::ThreadPool::start();
	
	Neo::BinaryScene binLoader;
	Neo::JsonScene jsonLoader;
	Neo::AssimpScene assimpLoader;
	Neo::glTFScene gltfLoader;

	Neo::LevelLoader::registerLoader(&binLoader);
	Neo::LevelLoader::registerLoader(&jsonLoader);
	Neo::LevelLoader::registerLoader(&gltfLoader);
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
	format.setSwapInterval(0);
	// format.setSamples(4);

//#ifdef NDEBUG
	if(Neo::Renderer::GraphicsDebugging())
	{
		LOG_INFO("Enabling OpenGl debug context");
		format.setOption(QSurfaceFormat::DebugContext);
	}
//#endif

	QSurfaceFormat::setDefaultFormat(format);
	
	try
	{	
		MainWindow w;
		w.show();

		return a.exec();
	}
	catch(std::exception& e)
	{
		LOG_ERROR("Uncaught exception: " << e.what());
		return 1;
	}
}
