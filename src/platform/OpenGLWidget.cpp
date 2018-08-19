#include "OpenGLWidget.h"

#include <iostream>
#include <QOpenGLContext>

#ifdef __linux__
#include <QtPlatformHeaders/QGLXNativeContext>
#endif

using namespace Neo;

OpenGLWidget::OpenGLWidget(QWidget* parent):
	QOpenGLWidget(parent)
{

}

void OpenGLWidget::initializeGL()
{
	
}

void OpenGLWidget::resizeGL(int w, int h)
{
	
}

void OpenGLWidget::paintGL()
{
	if(!m_render)
	{
#ifdef __linux__
		auto context = QOpenGLContext::currentContext();
		auto nativeContext = reinterpret_cast<QGLXNativeContext*>(context->nativeHandle().data());
#else
#error Unsupported platform!
#endif

		m_render = std::make_unique<PlatformRenderer>();
		m_render->initialize(width(), height(), nullptr, (void*) (uintptr_t) parentWidget()->winId(), nativeContext->context());
	}
	
	assert(m_render != nullptr);
	m_render->clear(1, 0, 1, 1);
	m_render->swapBuffers();
}
