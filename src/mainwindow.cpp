#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qsurface.h>

#include "platform/OpenGLWidget.h"

#include <Level.h>
#include <QFileDialog>
#include <Log.h>

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
	
	connect(this, &MainWindow::openLevel, [this](QString file) {
		ui->sceneEditor->getLevel()->load(file.toUtf8().data());
	});
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
}

void MainWindow::openLevelSlot()
{
	auto file = QFileDialog::getOpenFileName(this, tr("Open Level"), "/home", tr("Neo Level (*.nlv);;COLLADA DAE (*.dae)"));
	emit openLevel(file);
}
