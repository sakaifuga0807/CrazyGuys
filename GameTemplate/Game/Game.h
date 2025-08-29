#pragma once
#include "Level3DRender/LevelRender.h"

class BackGround;
class Player;
class GameCamera;

class Game : public IGameObject
{
public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	Player*			m_player;			//�v���C���[�B
	BackGround*		m_backGround;		//�X�e�[�W�B
	GameCamera*		m_gameCamera;		//�J�����B
};

