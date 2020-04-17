#ifndef NEO_GAMEWIDGET_H
#define NEO_GAMEWIDGET_H

#include "OpenGLWidget.h"
#include <Platform.h>
#include <LevelGameState.h>

#include <Object.h>
#include <behaviors/CameraBehavior.h>

namespace Neo 
{

class GameWidget : public OpenGLWidget
{
	Q_OBJECT;
public:
	GameWidget(QWidget* parent);
	
public slots:
	void playGame(LevelGameState*);
	void stopGame();
	void pauseGame();
	
	bool isPlaying() const { return m_game != nullptr; }
	LevelGameState* getGame() { return m_game; }

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
	
private:
	Platform m_platform;
	LevelGameState* m_game = nullptr;
	bool m_needsInit = false;
	
	Object m_cameraObject;
	CameraBehavior m_camera;
	
	class DummyWindow : public Window
	{
	public:
		DummyWindow(): Window(0, 0) {}
		void activateRendering() override {}
		void setRenderer(std::unique_ptr<Renderer> && renderer) override { Window::setRenderer(std::move(renderer), nullptr, nullptr, nullptr, nullptr); }
		void setTitle(const char * title) override {}
		void swapBuffers() override {}
	} m_window;
};

}

#endif
