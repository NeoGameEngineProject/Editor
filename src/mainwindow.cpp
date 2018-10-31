#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qsurface.h>

#include "platform/OpenGLWidget.h"

#include <Level.h>
#include <QFileDialog>
#include <QMessageBox>

#include <Log.h>
#include <Object.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setCentralWidget(nullptr);

	resetView();

	m_consoleStream.setOutput(ui->consoleOutput);
	Neo::Log::setOutStream(m_consoleStream);
	Neo::Log::setErrStream(m_consoleStream);
	
	auto level = std::make_shared<Neo::Level>();
	ui->sceneEditor->setLevel(level);
	ui->levelTree->setLevel(level);
	
	connect(this, &MainWindow::openLevel, [this](QString file) {

		LOG_INFO("Loading from file: " << file.toStdString());
		
		auto level = std::make_shared<Neo::Level>();
		ui->sceneEditor->setLevel(level);
		ui->levelTree->setLevel(level);

		if(file.endsWith(".nlv")) // Load the native binary format if possible
			ui->sceneEditor->getLevel()->loadBinary(file.toUtf8().data());
		else // Otherwise load compatible format as read-only to prevent accidental conversion
		{
			ui->sceneEditor->getLevel()->load(file.toUtf8().data());
			m_readOnly = true;
		}
		
		this->setWindowTitle(tr("Neo Editor") + " - " + file);
		m_file = file.toStdString();
	
		emit levelChanged();
	});
	
	connect(this, &MainWindow::saveLevel, [this](QString file) {
		
		LOG_INFO("Saving to file: " << file.toStdString());
		
		ui->sceneEditor->getLevel()->saveBinary(file.toUtf8().data());
		this->setWindowTitle(tr("Neo Editor") + " - " + file);

		m_file = file.toStdString();
		m_readOnly = false;
		
		emit levelChanged();
	});

	connect(this, &MainWindow::behaviorsChanged, [this]() mutable {
		LOG_DEBUG("Reloading behaviors");
		auto menu = ui->actionAdd_Behavior->menu();
		menu->clear();
		
		for(auto& b : Neo::Behavior::registeredBehaviors())
		{
			auto action = menu->addAction(b->getName());
			action->setObjectName(b->getName());
			
			connect(action, &QAction::triggered, [action, this]() mutable {
				auto name = action->objectName().toUtf8().data();
				LOG_DEBUG("Creating behavior " << name);
				
				for(auto obj : ui->sceneEditor->getSelection())
					obj->addBehavior(Neo::Behavior::create(name));
				
				emit levelChanged();
			});
		}
	});
	
	connect(this, &MainWindow::levelChanged, ui->levelTree, &Neo::LevelTreeWidget::levelChangedSlot);
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

	connect(ui->actionAdd_Object, &QAction::triggered, [this]() {
		
		auto level = ui->sceneEditor->getLevel();
		auto object = level->findInactive();
		
		if(object.empty())
			object = level->addObject("");

		object->getTransform().loadIdentity();
		object->updateFromMatrix();
		object->setActive(true);
	
		auto name = level->getUniqueName("Object");
		object->setName(name.c_str());
	
		level->getRoot()->addChild(object);
		object->setParent(level->getRoot());
		
		// To trigger re-init
		ui->sceneEditor->setLevel(level);
		emit levelChanged();
	});
	
	connect(ui->actionDelete_Object, &QAction::triggered, [this]() {
		auto& selection = ui->sceneEditor->getSelection();
		
		for(auto k : selection)
		{
			k->getBehaviors().clear();
			k->setActive(false);
			k->setName("");
			
			auto parent = k->getParent();
			if(!parent.empty())
				parent->removeChild(k);
		}
		
		ui->sceneEditor->setSelection({});
		
		// To trigger re-init
		ui->sceneEditor->setLevel(ui->sceneEditor->getLevel());
		emit levelChanged();
	});
	
	// Create behavior menu
	ui->actionAdd_Behavior->setMenu(new QMenu);
	
	emit behaviorsChanged();
	emit openLevel("/home/yannick/NeoDev/components/Editor/SDK/testgame/assets/test.dae");
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::resetView()
{
	splitDockWidget(ui->sceneDock, ui->editorDock, Qt::Orientation::Horizontal);
	splitDockWidget(ui->editorDock, ui->consoleDock, Qt::Orientation::Vertical);
	splitDockWidget(ui->sceneDock, ui->objectDock, Qt::Orientation::Vertical);

	tabifyDockWidget(ui->editorDock, ui->gameDock);

	ui->editorDock->raise();
	
	// Because resize does not work...
	ui->sceneDock->setMaximumWidth(0.15*width());
	ui->objectDock->setMaximumWidth(0.15*width());
	ui->consoleDock->setMaximumHeight(0.15*height());

	ui->sceneDock->setMaximumHeight(height() * 0.5);
	ui->objectDock->setMaximumHeight(height() * 0.5);
}

void MainWindow::resizeEvent(QResizeEvent* e)
{
	ui->sceneDock->setMaximumHeight(height());
	ui->objectDock->setMaximumHeight(height());

	ui->consoleDock->setMaximumHeight(ui->consoleDock->height());
	ui->sceneDock->setMaximumWidth(ui->sceneDock->width());
	ui->objectDock->setMaximumWidth(ui->objectDock->width());

	QMainWindow::resizeEvent(e);
	
	ui->sceneDock->setMaximumWidth(width());
	ui->objectDock->setMaximumWidth(width());
	ui->consoleDock->setMaximumHeight(height());
}

void MainWindow::openLevelSlot()
{
	auto file = QFileDialog::getOpenFileName(this, tr("Open Level"), ".", tr("Neo Level (*.nlv);;COLLADA DAE (*.dae)"));
	if(file.isEmpty())
		return;
	
	emit openLevel(file);
}

void MainWindow::saveLevelAsSlot()
{
	auto file = QFileDialog::getSaveFileName(this, tr("Save Level"), ".", tr("Neo Level (*.nlv)"));
	if(file.isEmpty())
		return;
	
	emit saveLevel(file);
}

void MainWindow::saveLevelSlot()
{
	if(m_readOnly)
	{
		QMessageBox::critical(this, tr("Save"), tr("Can not save: The file is loaded as read-only! Try 'Save As' to select another target."));
		return;
	}

	emit saveLevel(QString(m_file.c_str()));
}

void MainWindow::appendSceneSlot()
{
	auto level = ui->sceneEditor->getLevel();
	
	auto file = QFileDialog::getOpenFileName(this, tr("Open Level"), ".", tr("Neo Level (*.nlv);;COLLADA DAE (*.dae)"));
	if(file.isEmpty())
		return;
	
	auto name = file.toStdString();
	name = name.substr(name.find_last_of('/') + 1);
	
	level->makeNameUnique(name);
	
	auto obj = level->addObject(name.c_str());
	obj->setParent(level->getRoot());
	level->getRoot()->addChild(obj);
	
	level->load(file.toUtf8().data(), name.c_str());
	
	// To trigger re-init
	ui->sceneEditor->setLevel(level);
	emit levelChanged();
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

