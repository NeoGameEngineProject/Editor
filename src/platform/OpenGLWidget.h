#ifndef NEO_OPENGLWIDGET_H
#define NEO_OPENGLWIDGET_H

#include <QTimer>
#include <QOpenGLWidget>
#include <memory>
#include <functional>

#include <Vector4.h>
#include <PlatformRenderer.h>
#include <QMainWindow>

namespace Neo 
{

class Window;
class Platform;
class Renderer;

class OpenGLWidget : public QOpenGLWidget
{
	std::unique_ptr<PlatformRenderer> m_render;
	QTimer m_redrawTimer;

public:
	OpenGLWidget(QWidget* parent);

	void beginFrame();
	float endFrame();

	float getDeltaTime() const { return m_dt; }
	PlatformRenderer* getRenderer() { return m_render.get(); }

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

	long long m_frameBeginTime = 0;
	float m_dt = 0.0f;
};

}

#endif // NEO_OPENGLWIDGET_H
