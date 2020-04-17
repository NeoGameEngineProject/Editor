#include "GameWidget.h"
#include <Level.h>
#include <Log.h>

#include <QMessageBox>
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
	beginFrame();
	auto render = getRenderer();

	if(m_game)
	{
		if(m_needsInit)
		{
			try
			{
				m_window.setRenderer(m_platform.createRenderer());

				Level& level = m_game->getLevel();
				if(level.getCurrentCamera() == nullptr)
				{
					LOG_WARNING("Using fallback camera for game!");
					level.setCurrentCamera(&m_camera);
				}

				m_game->begin(m_platform, m_window);

				// Run first frame in try/catch block in case the loading procedures throw something
				m_game->update(m_platform, 1.0f/60.0f);
				m_game->draw(*render);
			}
			catch(std::exception& e)
			{
				m_needsInit = false;
				stopGame();

				QMessageBox::critical(this, tr("Frame Error"), tr("Error playing game: ") + e.what());
				return;
			}

			m_needsInit = false;
			render->swapBuffers();
			return;
		}
		
		render->clear(57.0f/255.0f, 57.0f/255.0f, 57.0f/255.0f, true);
		render->setBackbuffer((void*) defaultFramebufferObject());

		m_game->update(m_platform, 1.0f/60.0f);
		m_game->draw(*render);
	}
	else
	{
		render->clear(0, 0, 0, true);
	}

	render->swapBuffers();
	endFrame();
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
	repaint();
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
