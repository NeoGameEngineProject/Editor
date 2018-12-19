#include <MainGameState.h>
#include <Behavior.h>

using namespace Neo;

#ifdef NEO_PLUGIN
static MainGameState s_state;
extern "C" GameState* StartPlugin(std::vector<Behavior*>& behaviors)
{
	auto& globalBehaviors = Neo::Behavior::registeredBehaviors();
	behaviors.reserve(globalBehaviors.size());
	
	for(auto& ptr : globalBehaviors)
	{
		behaviors.push_back(ptr->getNew());
	}
	
	return &s_state;
}
#else
#include <WinMain.hpp>
#include <Game.h>

extern "C" int main(int argc, char** argv)
{
	Neo::Game game(1024, 768, "Neo Game");
	game.changeState(std::make_unique<MainGameState>());
	
	return game.run(argc, argv);
}

#endif
