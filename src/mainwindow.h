#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QUndoStack>

#include <memory>

#include <ConsoleStream.h>
#include <project/Project.h>
#include <plugins/PluginHost.h>

#include <behaviors/CameraBehavior.h>
#include <Level.h>

#include "UndoActions.h"

namespace Ui 
{
class MainWindow;
}

namespace Neo
{
class EditorWidget;
}

class QSettings;

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
	Neo::EditorWidget* getEditor();

	void importScene(bool asLink = true);

	template<typename Fn>
	void executeUndoableAction(Fn fn);

	template<typename Fn>
	auto createUndoableAction(Fn fn);

	// FIXME Ownership semantic! Who owns the pointer? Currently the caller!
	void registerInputMethod(std::shared_ptr<Neo::LuaScript> script) { m_inputMethods.push_back(script); }
	void selectInputMethod(size_t id);
	std::vector<std::shared_ptr<Neo::LuaScript>>& getInputMethods() { return m_inputMethods; }

	struct Configuration
	{
		std::string language = "en";
		std::string inputMethod = "";
		std::string theme = "Native";
		std::vector<std::string> pluginDirectories;

		void write(QSettings& settings);
		void read(QSettings& settings);
	};

	void applyConfiguration();

signals:
	void openLevel(QString file);
	void openProject(QString file);
	void createLevel(QString file);
	void createProject(QString file);
	
	void saveLevel(QString file);
	void saveProject(QString file);

	void beginUndoableChange();
	void endUndoableChange();

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

	void undoSlot();
	void redoSlot();

	void beginUndoableChangeSlot();
	void endUndoableChangeSlot();

	void translationTool();
	void rotationTool();
	void scaleTool();

	void publishGameSlot();
	void managePluginsSlot();
	void openPreferencesSlot();

	void closeEvent(QCloseEvent *event);
	void resetView();

private:
	Ui::MainWindow *ui;
	ConsoleStream m_consoleStream;
	std::unique_ptr<Project> m_currentProject;

	// Undo stack
	QUndoStack m_undoStack;

	// This is used when undo commands are created using signals
	QUndoCommand* m_currentUndoCommand = nullptr;
	
	std::string m_file; // The file that is currently being edited
	bool m_readOnly = false; // If the file is loaded as read-only (e.g. for DAE files)

	// Contains all registered input method scripts
	std::vector<std::shared_ptr<Neo::LuaScript>> m_inputMethods;

	// Current configuration
	Configuration m_config;
};

#endif // MAINWINDOW_H
