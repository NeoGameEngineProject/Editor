#ifndef NEO_LEVELWIDGET_H
#define NEO_LEVELWIDGET_H

#include "QtInputContext.h"
#include "OpenGLWidget.h"
#include <Platform.h>

#include <Object.h>
#include <behaviors/CameraBehavior.h>
#include <Log.h>
#include <LuaScript.h>

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

	void setNeedsInit(bool v)
	{
		m_levelNeedsInit = v;
	}

	std::shared_ptr<Level> getLevel() { return m_level; }
	CameraBehavior& getCamera() { return m_camera; }
	Platform& getPlatform() { return m_platform; }
	void begin(Behavior* b) { b->begin(m_platform, *getRenderer(), *m_level); }
	
	bool event(QEvent* e) override;

	void setMovementSpeed(float f) { m_movementSpeed = f; }
	float getMovementSpeed() const { return m_movementSpeed; }

	std::shared_ptr<Neo::LuaScript> getInputMethod() { return m_inputMethod; }
	void setInputMethod(std::shared_ptr<Neo::LuaScript> s) { m_inputMethod = s; }
	
protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

private:
	std::shared_ptr<Level> m_level;
	
	Platform m_platform;

	Object m_cameraObject;
	CameraBehavior m_camera;
	float m_movementSpeed = 1.0f; // Movement speed of the camera

	bool m_levelNeedsInit = false;

	// The current input script
	std::shared_ptr<Neo::LuaScript> m_inputMethod = nullptr;
};

}

#endif
