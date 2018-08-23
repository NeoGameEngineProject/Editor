#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

#include <memory>

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

signals:
	void openLevel(QString file);
	void openProject(QString file);
	
	void saveLevel(QString file);
	void saveProject(QString file);
	
public slots:
	void openLevelSlot();
	
private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
