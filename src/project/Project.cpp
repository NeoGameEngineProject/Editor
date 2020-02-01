#include "Project.h"

#include <fstream>
#include <thread>

#include <QDir>
#include <QApplication>
#include <QProcess>
#include <QtSql/QtSql>

#include <FileTools.h>
#include <Log.h>
#include <LevelGameState.h>
#include <Behavior.h>

static void runProcess(const QString& exe, const QStringList& args, const QString& workingDirectory = "")
{
	QProcess process;
	
	if(!workingDirectory.isEmpty())
		process.setWorkingDirectory(workingDirectory);
	
	process.setReadChannelMode(QProcess::ProcessChannelMode::MergedChannels);
	process.start(exe, args);
	process.waitForStarted();
	
	while(process.state() == QProcess::Running || process.canReadLine())
	{
		if(process.canReadLine())
		{
			std::string line = process.readLine().data();
			line.pop_back();
			
			if(!line.empty())
				LOG_INFO(line);
		}
		
		QApplication::processEvents();
	}
}

QString Project::buildDirectory(const QString& buildType)
{
	return QDir::currentPath() + QDir::separator() + "build-native-plugin-" + buildType;
}

void Project::enableCurrentDirectory()
{
	LOG_INFO("Switching CWD to: " << m_name.toStdString());
	QDir::setCurrent(m_name);
}

Project Project::create(const char* name)
{
	Project project;
	project.load(name, !isFileExist("project.nproj"));
	
	const auto appDir = QApplication::applicationDirPath();
	const auto input = appDir + QDir::separator() + "assets" + QDir::separator() + "templates" + QDir::separator() + "cmake";
	const auto output = QString(name) + QDir::separator() + "source";
	
	project.enableCurrentDirectory();
	copyDirectory(input.toUtf8().data(), output.toUtf8().data());
	createDirectory("assets");
	copyDirectory((appDir + QDir::separator() + "assets" + QDir::separator() + "glsl").toUtf8().data(), 
		      (QString(name) + QDir::separator() + "assets").toUtf8().data());
		
	project.buildDebug();
	
	return project;
}
	
void Project::buildDebug()
{
	// Configure, this also ensures all files have been globbed
	enableCurrentDirectory();
	configure("Debug");
	runProcess("cmake", QStringList() << "--build" << buildDirectory("Debug") << "--parallel" << std::to_string(std::thread::hardware_concurrency() + 1).c_str());
	reloadPlugin();
}

void Project::publish()
{
	enableCurrentDirectory();
}

void Project::reloadPlugin()
{
	if(m_plugin.isLoaded())
	{
		for(unsigned int i = m_behaviorIdxEnd; i >= m_behaviorIdxStart; i--)
		{
			Neo::Behavior::unregisterBehavior(i);
		}
		
		m_game = nullptr;
		m_plugin.unload();
	}
	
	if(!m_plugin.load())
	{
		LOG_ERROR("Could not load plugin: " << m_plugin.errorString().toUtf8().data());
		return;
	}
	
	auto start = reinterpret_cast<Neo::LevelGameState*(*)(std::vector<Neo::Behavior*>&)>(m_plugin.resolve("StartPlugin"));
	if(!start)
	{
		LOG_ERROR("Module does not have a start function!");
		return;
	}
	
	std::vector<Neo::Behavior*> behaviors;
	m_game = start(behaviors);
	
	m_behaviorIdxStart = Neo::Behavior::registeredBehaviors().size();
	for(auto* behavior : behaviors)
	{
		if(!Neo::Behavior::isBehaviorRegistered(behavior->getName()))
			Neo::Behavior::registerBehavior(std::unique_ptr<Neo::Behavior>(behavior));
	}
	m_behaviorIdxEnd = Neo::Behavior::registeredBehaviors().size() - 1;
	
	LOG_INFO("Plugin loaded");
}

void Project::load(const char* location, bool create)
{
	m_name = location;
	m_db = QSqlDatabase::addDatabase("QSQLITE");
	m_db.setDatabaseName(QString(location) + QDir::separator() + "project.nproj");
	
	if(!m_db.open())
		throw std::runtime_error("Could not open project!");
	
	if(create)
	{
		m_db.exec("create table Variables (key text primary key, value text)");
		m_db.exec("create table Thumbnails (file text primary key, time bigint, png blob)");
	}
	
	enableCurrentDirectory();
	
	m_plugin.setLoadHints(QLibrary::ResolveAllSymbolsHint);
	m_plugin.setFileName(buildDirectory("Debug") + QDir::separator() + "lib" + QDir::separator() + "libGamePlugin.so");
}

void Project::configure(const QString& buildType)
{
	enableCurrentDirectory();
	const auto buildDirectoryStr = buildDirectory(buildType);
	if(!isDirectory(buildDirectoryStr.toUtf8().data()))
		createDirectory(buildDirectoryStr.toUtf8().data(), true);

	const auto prefixPath = "-DNEO_ROOT=" + QDir(QApplication::applicationDirPath() + QDir::separator() + "..").absolutePath();
	
	LOG_INFO("Using Neo installation at: " << prefixPath.toUtf8().data());
	
	runProcess("cmake", QStringList() 
			<< "../source" 
			<< prefixPath
			<< "-DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE" 
			<< "-DDISABLE_MULTITHREAD=TRUE" 
			<< "-DNEO_PLUGIN=TRUE" 
			<< "-DCMAKE_BUILD_TYPE=" + buildType, buildDirectoryStr);
}
