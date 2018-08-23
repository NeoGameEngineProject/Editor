#include "OpenGLWidget.h"

#include <iostream>
#include <QOpenGLContext>

#ifdef __linux__
#include <QtPlatformHeaders/QGLXNativeContext>
#elif WIN32
#include <QtPlatformHeaders/QWGLNativeContext>
#endif

using namespace Neo;

OpenGLWidget::OpenGLWidget(QWidget* parent):
	QOpenGLWidget(parent),
	m_redrawTimer(this)
{
	connect(&m_redrawTimer, SIGNAL(timeout()), this, SLOT(update()));
	m_redrawTimer.start(1000.0f/60.0f);
}

void OpenGLWidget::initializeGL()
{
	/*
#ifdef __linux__
		auto context = QOpenGLContext::currentContext();
		auto nativeContext = reinterpret_cast<QGLXNativeContext*>(context->nativeHandle().data());
#elif WIN32
		auto context = QOpenGLContext::currentContext();
		auto nativeContext = reinterpret_cast<QWGLNativeContext*>(context->nativeHandle().data());
#else
#error Unsupported platform!
#endif
*/
		m_render = std::make_unique<PlatformRenderer>();
//		m_render->initialize(width(), height(), nullptr, (void*) (uintptr_t) parentWidget()->winId(), nativeContext->context());

		m_render->initialize(width(), height(), nullptr, nullptr, nullptr);
}

void OpenGLWidget::resizeGL(int w, int h)
{
	m_render->setViewport(0, 0, width(), height());
}

void OpenGLWidget::paintGL()
{
	assert(m_render != nullptr);
	m_render->clear(1, 0, 1, 1);
	m_render->swapBuffers();
}
