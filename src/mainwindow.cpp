#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qsurface.h>

#include "platform/OpenGLWidget.h"

#include <Level.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setCentralWidget(nullptr);

	resetView();

	auto level = std::make_shared<Neo::Level>();
	ui->sceneEditor->setLevel(level);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::resetView()
{
	splitDockWidget(ui->sceneDock, ui->editorDock, Qt::Orientation::Horizontal);
	splitDockWidget(ui->sceneDock, ui->objectDock, Qt::Orientation::Vertical);

	tabifyDockWidget(ui->editorDock, ui->gameDock);

	ui->editorDock->raise();

	ui->editorDock->setMinimumWidth(width()*0.90);
}
