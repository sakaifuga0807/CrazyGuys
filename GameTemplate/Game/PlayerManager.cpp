#include "stdafx.h"
#include "PlayerManager.h"
#include "Player.h"
#include "Game.h"

void PlayerManager::CreatePlayers(Game* game)
{
	const char* models[] = {
	"Assets/modelData/BlueGuys.tkm",
	"Assets/modelData/PinkGuys.tkm",
	"Assets/modelData/WhiteGuys.tkm",
	"Assets/modelData/Penguin.tkm"
	};


	std::vector<int> pads;
	for (int i = 0; i < 4; i++)
		if (g_pad[i] && g_pad[i]->IsConnected()) pads.push_back(i);


	for (int i = 0; i < 4; i++)
	{
		auto p = NewGO<Player>(0, ("Player" + std::to_string(i)).c_str());
		p->SetModelPath(models[i % 4]);
		p->SetControllerIndex(i < pads.size() ? pads[i] : -1);
		p->SetPosition({ -400.0f + i * 200.0f,0,100 });
		p->SetGame(game);
		m_players.push_back(p);
	}
}