#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qsurface.h>

#include "platform/OpenGLWidget.h"
#include "project/Project.h"

#include <Level.h>
#include <QFileDialog>
#include <QMessageBox>

#include <Log.h>
#include <Object.h>
#include <LevelGameState.h>

using namespace std;

static Neo::ObjectHandle createObject(Neo::Level& level, const char* newName)
{
	auto object = level.findInactive();
	
	if(object.empty())
		object = level.addObject("");

	object->getTransform().loadIdentity();
	object->updateFromMatrix();
	object->setActive(true);

	auto name = level.getUniqueName(newName);
	object->setName(name.c_str());

	level.getRoot()->addChild(object);
	object->setParent(level.getRoot());
	
	return object;
}

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
		
		// Disable physics simulation in the editor
		level->getPhysicsContext().setEnabled(false);
		
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
	
	connect(this, &MainWindow::createProject, [this](QString file) {

		LOG_INFO("Creating project in: " << file.toStdString());
		m_currentProject = std::make_unique<Project>(std::move(Project::create(file.toUtf8().data())));
	});
	
	connect(this, &MainWindow::openProject, [this](QString file) {
		LOG_INFO("Creating project in: " << file.toStdString());
		m_currentProject = std::make_unique<Project>(file.toUtf8().data());
		m_currentProject->buildDebug();
		emit behaviorsChanged();
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
			if(!b->isEditorVisible())
				continue;
			
			auto action = menu->addAction(b->getName());
			action->setObjectName(b->getName());
			
			connect(action, &QAction::triggered, [action, this]() mutable {
				
				auto& selection = ui->sceneEditor->getSelection();
				if(selection.empty())
					return;
						
				auto name = action->objectName().toStdString();
				LOG_DEBUG("Creating behavior " << name);
				
				for(auto obj : selection)
				{
					if(obj->getBehavior(name))
					{
						LOG_WARNING("Behavior was already registered for object " << obj->getName());
						continue;
					}
						
					Neo::Behavior* behavior = obj->addBehavior(Neo::Behavior::create(name.c_str()));
					ui->sceneEditor->begin(behavior);
				}
				
				ui->objectWidget->setObject(selection.front());
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
	
	connect(ui->actionEmpty, &QAction::triggered, [this]() {
		
		createObject(*ui->sceneEditor->getLevel(), "Object");
		
		// To trigger re-init
		ui->sceneEditor->setLevel(ui->sceneEditor->getLevel());
		emit levelChanged();
	});
	
	connect(ui->actionCamera, &QAction::triggered, [this]() {
		
		auto cam = createObject(*ui->sceneEditor->getLevel(), "Camera");
		cam->addBehavior<Neo::CameraBehavior>();
		
		// To trigger re-init
		ui->sceneEditor->setLevel(ui->sceneEditor->getLevel());
		emit levelChanged();
	});
	
	connect(ui->actionLight, &QAction::triggered, [this]() {
		
		auto obj = createObject(*ui->sceneEditor->getLevel(), "Light");
		obj->addBehavior<Neo::LightBehavior>();
		
		// To trigger re-init
		ui->sceneEditor->setLevel(ui->sceneEditor->getLevel());
		emit levelChanged();
	});
	
	connect(ui->actionSound, &QAction::triggered, [this]() {
		
		auto file = QFileDialog::getOpenFileName(this, tr("Open Level"), ".", tr("Wave File (*.wav);;OGG(*.ogg)"));
		if(file.isEmpty())
			return;
		
		auto level = ui->sceneEditor->getLevel();
		auto sound = level->loadSound(file.toUtf8().data());
		if(sound.empty())
		{
			QMessageBox::critical(this, tr("Load Sound"), tr("Could not load sound file!"));
			return;
		}
		
		auto obj = createObject(*level, "Sound");
		obj->addBehavior(std::make_unique<Neo::SoundBehavior>(sound));
		
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
	
	connect(ui->actionPlay, &QAction::triggered, [this]() {
		
		if(ui->gamePlayer->isPlaying())
		{
			emit behaviorsChanged();

			auto* game = ui->gamePlayer->getGame();
			if(m_currentProject != nullptr)
				delete game;

			ui->gamePlayer->stopGame();
			return;
		}

		Neo::LevelGameState* game = nullptr;
		if(m_currentProject != nullptr)
		{
			m_currentProject->buildDebug();
			game = m_currentProject->getGameState();
		}
		else
		{
			game = new Neo::LevelGameState();
		}
			
		// Load scene into buffer and and load into the game
		// This ensures all behaviors are loaded correctly
		// TODO Shallow copy so only objects are duplicated but not assets!
		{
			std::stringstream buffer;
			ui->sceneEditor->getLevel()->serialize(buffer);
			game->getLevel().deserialize(buffer);
		}
		
		ui->gamePlayer->playGame(game);
					
		emit behaviorsChanged();
		emit playGame();
	});
	
	emit openLevel("/home/yannick/NeoDev/components/Editor/SDK/testgame/assets/test.dae");
	
	// Create behavior menu
	ui->actionAdd_Behavior->setMenu(new QMenu);
	emit behaviorsChanged();
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
	auto file = QFileDialog::getOpenFileName(this, tr("Open Level"), ".", tr("All Supported (*.nlv *.dae *.3ds *.obj);;Neo Level (*.nlv);;Collada DAE (*.dae)"));
	if(file.isEmpty())
		return;
	
	emit openLevel(file);
}

void MainWindow::createProjectSlot()
{
	auto file = QFileDialog::getExistingDirectory(this, tr("Select Project Directory"), ".");
	if(file.isEmpty())
		return;
	
	emit createProject(file);
}

void MainWindow::openProjectSlot()
{
	auto file = QFileDialog::getExistingDirectory(this, tr("Select Project Directory"), ".");
	if(file.isEmpty())
		return;
	
	emit openProject(file);
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

