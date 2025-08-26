#pragma once
#include "Level3DRender/LevelRender.h"

class BackGround;
class Player;

class Game : public IGameObject
{
public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	Player* m_player;//プレイヤー。
	BackGround* m_backGround;//ステージ。
};

