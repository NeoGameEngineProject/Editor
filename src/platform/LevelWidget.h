#ifndef NEO_LEVELWIDGET_H
#define NEO_LEVELWIDGET_H

#include "OpenGLWidget.h"
#include <Platform.h>

#include <Object.h>
#include <behaviors/CameraBehavior.h>

namespace Neo 
{

class LevelWidget : public OpenGLWidget
{
public:
	LevelWidget(QWidget* parent);

	void setLevel(const std::shared_ptr<Level>& level)
	{
		m_level = level;
		m_levelNeedsInit = true;
	}

	std::shared_ptr<Level> getLevel() { return m_level; }

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

private:
	std::shared_ptr<Level> m_level;
	Platform m_platform;

	Object m_cameraObject;
	CameraBehavior m_camera;

	bool m_levelNeedsInit = false;
};

}

#endif
