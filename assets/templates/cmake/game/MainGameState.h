#ifndef MAINGAMESTATE_H
#define MAINGAMESTATE_H

#include <LevelGameState.h>

class MainGameState : public Neo::LevelGameState
{
public:
	void begin(Neo::Platform & p, Neo::Window& w) override;
	void draw(Neo::Renderer& r) override;
	void update(Neo::Platform & p, float dt) override;
	void end();
};

#endif
