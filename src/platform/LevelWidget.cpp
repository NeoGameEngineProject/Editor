#include "LevelWidget.h"
#include <Level.h>
#include <Platform.h>

using namespace Neo;

LevelWidget::LevelWidget(QWidget* parent):
	OpenGLWidget(parent),
	m_level(std::make_shared<Level>())
{
	m_camera.setParent(&m_cameraObject);
}

void LevelWidget::initializeGL()
{
	OpenGLWidget::initializeGL();
	m_levelNeedsInit = true;
}

void LevelWidget::resizeGL(int w, int h)
{
	OpenGLWidget::resizeGL(w, h);
}

void LevelWidget::paintGL()
{
	if(m_levelNeedsInit)
	{
		m_level->begin(m_platform, *getRenderer());
		m_level->setCurrentCamera(&m_camera);
	}

	m_level->update(m_platform, 0.0f);

	auto render = getRenderer();
	render->clear(57.0f/255.0f, 57.0f/255.0f, 57.0f/255.0f, true);
	m_level->draw(*render);
	render->swapBuffers();
}
