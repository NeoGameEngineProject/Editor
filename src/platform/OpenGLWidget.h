#ifndef NEO_OPENGLWIDGET_H
#define NEO_OPENGLWIDGET_H

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
public:
	OpenGLWidget(QWidget* parent);

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

	PlatformRenderer* getRenderer() { return m_render.get(); }
};

}

#endif // NEO_OPENGLWIDGET_H
