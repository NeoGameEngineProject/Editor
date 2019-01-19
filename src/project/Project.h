#ifndef PROJECT_H
#define PROJECT_H

#include <QtSql/QSqlDatabase>
#include <QLibrary>

namespace Neo { class LevelGameState; }

class Project
{
	QLibrary m_plugin;
	QSqlDatabase m_db;
	QString m_name;
	
	QString buildDirectory(const QString& buildType);
	
	size_t m_behaviorIdxStart = 0, m_behaviorIdxEnd = 0;
	
	Neo::LevelGameState* m_game = nullptr;
	
public:
	/**
	 * 1) Create CMakeLists.txt which includes Neo either as an ExternalModule or from the system with find_package
	 * 2) Create SQLite DB with config about CMake, compiler, assets etc.
	 **/
	static Project create(const char* name);
	
	Project() = default;
	Project(const char* location) { load(location); }
	Project(const Project& project)
	{
		load(project.m_name.toUtf8().data());
	}
	
	void reloadPlugin();
	void load(const char* location, bool create = false);
	
	void configure(const QString& buildType = "Release");
	void buildDebug();
	void publish();
	
	void enableCurrentDirectory();
	
	Neo::LevelGameState* getGameState() { return m_game; }
};

#endif // PROJECT_H
