#include "GameWidget.h"
#include <Level.h>
#include <Log.h>

#include <QApplication>

using namespace Neo;

GameWidget::GameWidget(QWidget* parent):
	OpenGLWidget(parent)
{
	m_camera.setParent(&m_cameraObject);
}

void GameWidget::initializeGL()
{
	OpenGLWidget::initializeGL();
}

void GameWidget::resizeGL(int w, int h)
{
	OpenGLWidget::resizeGL(w, h);
}

void GameWidget::paintGL()
{
	OpenGLWidget::paintGL();
	if(m_game)
	{
		if(m_needsInit)
		{
			m_window.setRenderer(m_platform.createRenderer());

			Level& level = m_game->getLevel();
			if(level.getCurrentCamera() == nullptr)
			{
				LOG_WARNING("Using fallback camera for game!");
				level.setCurrentCamera(&m_camera);
			}
			
			m_game->begin(m_platform, m_window);
			m_needsInit = false;
		}
		
		m_game->update(m_platform, 1.0f/60.0f);
		m_game->draw(*m_window.getRenderer());
	}
}

void GameWidget::playGame(LevelGameState* state)
{
	LOG_DEBUG("Play game");
	if(state == nullptr)
	{
		stopGame();
		return;
	}
	
	m_game = state;
	m_needsInit = true;
}

void GameWidget::stopGame()
{
	LOG_DEBUG("Stop game");
	
	if(m_game)
	{
		m_game->end();
		m_game = nullptr;
	}
}

void GameWidget::pauseGame()
{
	LOG_DEBUG("Pause game");
}
