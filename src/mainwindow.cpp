#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qsurface.h>

#include "platform/OpenGLWidget.h"

#include <Level.h>
#include <QFileDialog>
#include <QMessageBox>

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
	ui->levelTree->setLevel(level);
	
	connect(this, &MainWindow::openLevel, [this](QString file) {
		ui->sceneEditor->getLevel()->load(file.toUtf8().data());
		emit levelChanged();
	});
	
	connect(ui->levelTree, &Neo::LevelTreeWidget::objectSelectionListChanged, ui->sceneEditor, &Neo::EditorWidget::setSelection);
	connect(ui->sceneEditor, &Neo::EditorWidget::objectSelectionListChanged, ui->levelTree, &Neo::LevelTreeWidget::setSelectionList);
	connect(ui->sceneEditor, &Neo::EditorWidget::objectChanged, ui->objectWidget, &Neo::ObjectWidget::updateObject);

	connect(ui->objectWidget, &Neo::ObjectWidget::requestNameChange, [this, level](Neo::ObjectHandle h, QString name) {
		if(name.isEmpty())
		{
			QMessageBox::critical(this, tr("Rename"), tr("Could not rename object: Name is empty!"));
			return;
		}
		
		auto nameData = name.toUtf8().data();
		auto find = level->find(nameData);
		
		if(find == h)
			return;
		else if(!find.empty())
		{
			QMessageBox::critical(this, tr("Rename"), tr("Could not rename object: Object with that name already exists!"));
			return;
		}
		
		h->setName(nameData);
		ui->levelTree->levelChangedSlot();
		ui->objectWidget->updateObject(h);
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
	
	// Because resize does not work...
	ui->sceneDock->setMaximumWidth(0.15*width());
	ui->objectDock->setMaximumWidth(0.15*width());
	
	ui->sceneDock->setMaximumHeight(height() * 0.5);
	ui->objectDock->setMaximumHeight(height() * 0.5);
}

void MainWindow::resizeEvent(QResizeEvent* e)
{
	ui->sceneDock->setMaximumHeight(height());
	ui->objectDock->setMaximumHeight(height());
	
	ui->sceneDock->setMaximumWidth(ui->sceneDock->width());
	ui->objectDock->setMaximumWidth(ui->objectDock->width());
	
	QMainWindow::resizeEvent(e);
	
	ui->sceneDock->setMaximumWidth(width());
	ui->objectDock->setMaximumWidth(width());
}

void MainWindow::openLevelSlot()
{
	auto file = QFileDialog::getOpenFileName(this, tr("Open Level"), "/home", tr("Neo Level (*.nlv);;COLLADA DAE (*.dae)"));
	emit openLevel(file);
}

void MainWindow::translationTool()
{
	ui->sceneEditor->setMode(Neo::EDITOR_TRANSLATE);
}

void MainWindow::rotationTool()
{
	ui->sceneEditor->setMode(Neo::EDITOR_ROTATE);
}

void MainWindow::scaleTool()
{
	ui->sceneEditor->setMode(Neo::EDITOR_SCALE);
}

