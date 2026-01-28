#pragma once

class Game;
class Player;

class PlayerManager:public IGameObject
{
public:
	void CreatePlayers(Game* game);
	const std::vector<Player*>GetPlayers()const { return m_players; }
private:
	std::vector<Player*> m_players;
};

