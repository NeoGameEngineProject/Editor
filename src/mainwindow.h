#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

#include <memory>

#include <ConsoleStream.h>
#include <project/Project.h>
#include <plugins/PluginHost.h>

#include <behaviors/CameraBehavior.h>
#include <Level.h>

namespace Ui 
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	
	void readSettings();
	void resizeEvent(QResizeEvent* e) override;
	
	Neo::Level& getEditorLevel();
	Neo::CameraBehavior& getEditorCamera();

	void importScene(bool asLink = true);

signals:
	void openLevel(QString file);
	void openProject(QString file);
	void createLevel(QString file);
	void createProject(QString file);
	
	void saveLevel(QString file);
	void saveProject(QString file);
	
	void levelChanged();
	void behaviorsChanged();
	
	void playGame();
	
public slots:
	void openLevelSlot();
	void saveLevelSlot();
	void saveLevelAsSlot();
	
	void importSceneSlot();
	void importSceneAsLinkSlot();

	void createProjectSlot();
	void openProjectSlot();
	
	void translationTool();
	void rotationTool();
	void scaleTool();

	void publishGameSlot();
	void managePluginsSlot();

	void closeEvent(QCloseEvent *event);
	void resetView();

private:
	Ui::MainWindow *ui;
	ConsoleStream m_consoleStream;
	std::unique_ptr<Project> m_currentProject;
	
	std::string m_file; // The file that is currently being edited
	bool m_readOnly = false; // If the file is loaded as read-only (e.g. for DAE files)
};

#endif // MAINWINDOW_H
