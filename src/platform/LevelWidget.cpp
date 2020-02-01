#include "LevelWidget.h"
#include <Level.h>
#include <Platform.h>
#include <Log.h>

#include "QtInputContext.h"

#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMessageBox>

using namespace Neo;

LevelWidget::LevelWidget(QWidget* parent):
	OpenGLWidget(parent),
	m_level(std::make_shared<Level>())
{
	m_camera.setParent(&m_cameraObject);
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
}

void LevelWidget::initializeGL()
{
	OpenGLWidget::initializeGL();
	m_levelNeedsInit = true;
	
	m_cameraObject.setPosition(Vector3(0, 0, 5));
}

void LevelWidget::resizeGL(int w, int h)
{
	OpenGLWidget::resizeGL(w, h);
}

void LevelWidget::paintGL()
{
	auto& input = m_platform.getInputContext();

	if(hasFocus())
	{
		if(input.isKeyDown(Neo::KEY_W))
			m_cameraObject.translate(Neo::Vector3(0, 0, -1), true);
		else if(input.isKeyDown(Neo::KEY_S))
			m_cameraObject.translate(Neo::Vector3(0, 0, 1), true);
		
		if(input.isKeyDown(Neo::KEY_A))
			m_cameraObject.translate(Neo::Vector3(-1, 0, 0), true);
		else if(input.isKeyDown(Neo::KEY_D))
			m_cameraObject.translate(Neo::Vector3(1, 0, 0), true);
		
		if(input.getMouse().isKeyDown(Neo::MOUSE_BUTTON_RIGHT))
		{
			auto rotation = m_cameraObject.getRotation().getEulerAngles();
			rotation.z -= input.getMouse().getDirection().x * 0.1;
			rotation.x -= input.getMouse().getDirection().y * 0.1;
			m_cameraObject.setRotation(Neo::Quaternion(rotation.x, rotation.y, rotation.z));
		}
	}
	input.getMouse().setDirection(Vector2());
	
	m_cameraObject.updateMatrix();
	m_camera.enable(width(), height());
	
	if(m_levelNeedsInit)
	{
		try
		{
			m_level->begin(m_platform, *getRenderer());
		}
		catch(const std::exception& e)
		{
			LOG_ERROR("Could not begin level: " << e.what());
			QMessageBox::critical(this, tr("Could not begin Level"), e.what());

			m_level->end();
			m_levelNeedsInit = false;
		}

		m_level->setCurrentCamera(&m_camera);
		m_levelNeedsInit = false;
	}

	m_level->update(m_platform, 1.0f/60.0f);

	auto render = getRenderer();
	render->clear(57.0f/255.0f, 57.0f/255.0f, 57.0f/255.0f, true);
	render->setCurrentFBO(defaultFramebufferObject());
	m_level->draw(*render);
	render->swapBuffers();
}

namespace
{

INPUT_KEYS translateEvent(int key)
{
	switch (key)
	{
		case Qt::Key_A:
			return KEY_A;
		case Qt::Key_B:
			return KEY_B;
		case Qt::Key_C:
			return KEY_C;
		case Qt::Key_D:
			return KEY_D;
		case Qt::Key_E:
			return KEY_E;
		case Qt::Key_F:
			return KEY_F;
		case Qt::Key_G:
			return KEY_G;
		case Qt::Key_H:
			return KEY_H;
		case Qt::Key_I:
			return KEY_I;
		case Qt::Key_J:
			return KEY_J;
		case Qt::Key_K:
			return KEY_K;
		case Qt::Key_L:
			return KEY_L;
		case Qt::Key_M:
			return KEY_M;
		case Qt::Key_N:
			return KEY_N;
		case Qt::Key_O:
			return KEY_O;
		case Qt::Key_P:
			return KEY_P;
		case Qt::Key_Q:
			return KEY_Q;
		case Qt::Key_R:
			return KEY_R;
		case Qt::Key_S:
			return KEY_S;
		case Qt::Key_T:
			return KEY_T;
		case Qt::Key_U:
			return KEY_U;
		case Qt::Key_V:
			return KEY_V;
		case Qt::Key_W:
			return KEY_W;
		case Qt::Key_X:
			return KEY_X;
		case Qt::Key_Y:
			return KEY_Y;
		case Qt::Key_Z:
			return KEY_Z;

#if 0
		case Qt::Key_0:
			return KEY_KP0;
		case Qt::Key_ _KP_1:
			return KEY_KP1;
		case Qt::Key_KP_2:
			return KEY_KP2;
		case Qt::Key_KP_3:
			return KEY_KP3;
		case Qt::Key_KP_4:
			return KEY_KP4;
		case Qt::Key_KP_5:
			return KEY_KP5;
		case Qt::Key_KP_6:
			return KEY_KP6;
		case Qt::Key_KP_7:
			return KEY_KP7;
		case Qt::Key_KP_8:
			return KEY_KP8;
		case Qt::Key_KP_9:
			return KEY_KP9;
#endif
			
		case Qt::Key_0:
			return KEY_0;
		case Qt::Key_1:
			return KEY_1;
		case Qt::Key_2:
			return KEY_2;
		case Qt::Key_3:
			return KEY_3;
		case Qt::Key_4:
			return KEY_4;
		case Qt::Key_5:
			return KEY_5;
		case Qt::Key_6:
			return KEY_6;
		case Qt::Key_7:
			return KEY_7;
		case Qt::Key_8:
			return KEY_8;
		case Qt::Key_9:
			return KEY_9;

#if 0
		case Qt::Key_KP_ENTER:
			return KEY_KP_ENTER;
#endif
		case Qt::Key_Space:
			return KEY_SPACE;
		case Qt::Key_Escape:
			return KEY_ESCAPE;
		case Qt::Key_Tab:
			return KEY_TAB;
			
		case Qt::Key_Shift:
			return KEY_LSHIFT;
#if 0
		case Qt::Key_RSHIFT:
			return KEY_RSHIFT;
#endif
			
		case Qt::Key_Control:
			return KEY_LCONTROL;
		
#if 0
		case Qt::Key_RCTRL:
			return KEY_RCONTROL;
#endif
			
		case Qt::Key_Alt:
			return KEY_LALT;

		case Qt::Key_Mode_switch:
		case Qt::Key_AltGr:
			return KEY_RALT;
		case Qt::Key_Super_L:
			return KEY_LSUPER;
		case Qt::Key_Super_R:
			return KEY_RSUPER;
		case Qt::Key_Menu:
			return KEY_MENU;
		case Qt::Key_NumLock:
			return KEY_NUMLOCK;
		case Qt::Key_Pause:
			return KEY_PAUSE;
		case Qt::Key_Delete:
			return KEY_DELETE;
		case Qt::Key_Backspace:
			return KEY_BACKSPACE;
		case Qt::Key_Return:
			return KEY_RETURN;
		case Qt::Key_Home:
			return KEY_HOME;
		case Qt::Key_End:
			return KEY_END;
		case Qt::Key_PageUp:
			return KEY_PAGEUP;
		case Qt::Key_PageDown:
			return KEY_PAGEDOWN;
		case Qt::Key_Insert:
			return KEY_INSERT;

		case Qt::Key_Left:
			return KEY_LEFT_ARROW;
		case Qt::Key_Right:
			return KEY_RIGHT_ARROW;
		case Qt::Key_Down:
			return KEY_DOWN_ARROW;
		case Qt::Key_Up:
			return KEY_UP_ARROW;

		case Qt::Key_F1:
			return KEY_F1;
		case Qt::Key_F2:
			return KEY_F2;
		case Qt::Key_F3:
			return KEY_F3;
		case Qt::Key_F4:
			return KEY_F4;
		case Qt::Key_F5:
			return KEY_F5;
		case Qt::Key_F6:
			return KEY_F6;
		case Qt::Key_F7:
			return KEY_F7;
		case Qt::Key_F8:
			return KEY_F8;
		case Qt::Key_F9:
			return KEY_F9;
		case Qt::Key_F10:
			return KEY_F10;
		case Qt::Key_F11:
			return KEY_F11;
		case Qt::Key_F12:
			return KEY_F12;

		default:
			break;
	}

	return KEY_DUMMY;
}
}

bool LevelWidget::event(QEvent* e)
{
	auto& input = m_platform.getInputContext();

	switch(e->type())
	{
		case QEvent::KeyPress:
		{
			QKeyEvent* key = static_cast<QKeyEvent*>(e);
			input.getKeyboard().keyDown(translateEvent(key->key()));
		}
		break;
		
		case QEvent::KeyRelease:
		{
			QKeyEvent* key = static_cast<QKeyEvent*>(e);
			input.getKeyboard().keyUp(translateEvent(key->key()));
		}
		break;
		
		case QEvent::MouseMove:
		{
			QMouseEvent* mouse = static_cast<QMouseEvent*>(e);
			input.getMouse().moveCursor(Vector2(mouse->x(), mouse->y()));
		}
		break;
			
		case QEvent::MouseButtonPress:
		{
			QMouseEvent* mouse = static_cast<QMouseEvent*>(e);
			switch(mouse->button())
			{
				case Qt::LeftButton:
					input.getMouse().keyDown(MOUSE_BUTTON_LEFT);
				break;
				case Qt::RightButton:
					input.getMouse().keyDown(MOUSE_BUTTON_RIGHT);
				break;
				case Qt::MiddleButton:
					input.getMouse().keyDown(MOUSE_BUTTON_MIDDLE);
				break;
			}
		}
		break;
			
		case QEvent::MouseButtonRelease:
		{
			QMouseEvent* mouse = static_cast<QMouseEvent*>(e);
			switch(mouse->button())
			{
				case Qt::LeftButton:
					input.getMouse().keyUp(MOUSE_BUTTON_LEFT);
				break;
				case Qt::RightButton:
					input.getMouse().keyUp(MOUSE_BUTTON_RIGHT);
				break;
				case Qt::MiddleButton:
					input.getMouse().keyUp(MOUSE_BUTTON_MIDDLE);
				break;
			}
		}
		break;
			
		default:
			return OpenGLWidget::event(e);
	}
	
	if(!input.isMouseRelative()) // Only calculate direction from position when it is needed
		input.getMouse().flushDirection();
	
	e->accept();
	return true;
}
