#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

#include <memory>

#include <ConsoleStream.h>

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
	
	void resetView();

	void resizeEvent(QResizeEvent* e) override;
	
signals:
	void openLevel(QString file);
	void openProject(QString file);
	
	void saveLevel(QString file);
	void saveProject(QString file);
	
	void levelChanged();
	
public slots:
	void openLevelSlot();
	void saveLevelSlot();
	void saveLevelAsSlot();

	void translationTool();
	void rotationTool();
	void scaleTool();
	
private:
	Ui::MainWindow *ui;
	ConsoleStream m_consoleStream;
	
	std::string m_file; // The file that is currently being edited
	bool m_readOnly = false; // If the file is loaded as read-only (e.g. for DAE files)
};

#endif // MAINWINDOW_H
