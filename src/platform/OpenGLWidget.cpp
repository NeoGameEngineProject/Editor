#include "OpenGLWidget.h"

#include <Log.h>
#include <QOpenGLContext>
#include <QApplication>
#include <QDir>

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

	m_redrawTimer.setTimerType(Qt::TimerType::PreciseTimer);
	m_redrawTimer.setSingleShot(true);
	m_redrawTimer.start(1000.0f/m_fps);
}

void OpenGLWidget::initializeGL()
{
#ifdef __linux__
		auto context = QOpenGLContext::currentContext();
		auto nativeContext = reinterpret_cast<QGLXNativeContext*>(context->nativeHandle().data())->context();
#elif WIN32
		auto context = QOpenGLContext::currentContext();
		auto nativeContext = reinterpret_cast<QWGLNativeContext*>(context->nativeHandle().data())->context();
#elif __APPLE__
		// FIXME Native context is only available in ObjC...
		auto context = QOpenGLContext::currentContext();
		auto nativeContext = nullptr;
#else
#error Unsupported platform!
#endif

		m_render = std::make_unique<PlatformRenderer>();
		m_render->initialize(width(), height(), (void*) static_cast<uintptr_t>(context->defaultFramebufferObject()), nullptr, (void*) winId(), nativeContext);

//		m_render->initialize(width(), height(), nullptr, nullptr, nullptr);
}

void OpenGLWidget::resizeGL(int w, int h)
{
	auto context = QOpenGLContext::currentContext();

	// Update default FBO as it most certainly will have changed
	// due to the resize!
	m_render->setBackbuffer((void*) static_cast<uintptr_t>(context->defaultFramebufferObject()));
	m_render->setViewport(0, 0, w, h);
}

void OpenGLWidget::paintGL()
{
	assert(m_render != nullptr);
	m_render->clear(1, 0, 1, 1);
	m_render->swapBuffers();
}

void OpenGLWidget::beginFrame()
{
	using namespace std;
	using namespace chrono;
	m_frameBeginTime = duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
}

float OpenGLWidget::endFrame()
{
	using namespace std;
	using namespace chrono;
	const auto t = duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
	const auto dt = t - m_frameBeginTime;
	m_dt = static_cast<float>(dt) / 1000.0f;

	const auto sleepTime = std::max(1, static_cast<int>(std::floor((1000.0f / m_fps) - m_dt)));
	m_redrawTimer.start(sleepTime);
	
	return m_dt;
}
