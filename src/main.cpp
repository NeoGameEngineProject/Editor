#include <Platform.h>

#include <iostream>
#include <InputContext.h>
#include <Level.h>
#include <behaviors/CameraBehavior.h>
#include <InputContext.h>

#include <VFSFile.h>

#include <HTMLView.h>
#include <Game.h>
#include <SplashScreen.h>

class TemplateGame : public Neo::GameState
{
public:
	void begin(Neo::Platform & p, Neo::Window& w) override
	{
		
	}
	
	void draw(Neo::Renderer& r) override
	{
		r.clear(57.0f/255.0f, 57.0f/255.0f, 57.0f/255.0f, true);
		
		r.swapBuffers();
	}
	
	void update(Neo::Platform & p, float dt) override
	{
		auto& input = p.getInputContext();
		input.handleInput();
	}
	
	void end() override
	{
		
	}
};

extern "C" int main(int argc, char** argv)
{
	Neo::Game game(1024, 768, "Template Game");

	auto testGame = std::make_unique<TemplateGame>();
	auto splash = new Neo::States::SplashScreen(std::move(testGame), 2, 1, { "assets/Splash.png" });
	game.changeState(std::unique_ptr<Neo::States::SplashScreen>(splash));
	return game.run();
}
