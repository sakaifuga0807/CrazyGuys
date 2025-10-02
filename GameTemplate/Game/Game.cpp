#include "stdafx.h"
#include "Game.h"
#include "BackGround.h"
#include "Player.h"
#include "GameCamera.h"
#include "Goal.h"

bool Game::Start()
{
	//ステージを生成。
	m_backGround=NewGO<BackGround>(0,"background");
	//プレイヤーを生成。
	m_player=NewGO<Player>(0,"player");
	//カメラを生成。
	m_gameCamera = NewGO<GameCamera>(0, "gamecamera");
	//ゴールの生成。
	m_goal = NewGO<Goal>(0, "goal");
	//スカイキューブを作成
	m_skyCube = NewGO<SkyCube>(0, "skycube");
	m_skyCube->SetType(enSkyCubeType_DayToon);
	m_skyCube->SetScale(10000.0f);
	//m_skyCube->SetPosition(Vector3(0.0f, 0.0f, 0.0f));

	return true;
}

Game::Game()
{

}

Game::~Game()
{
	DeleteGO(m_backGround);
	DeleteGO(m_player);
	DeleteGO(m_gameCamera);
	DeleteGO(m_goal);
	DeleteGO(m_skyCube);
}

void Game::Update()
{
	if (m_isDelete)
	{
		DeleteGO(this);
	}
}

void Game::Render(RenderContext& rc)
{

}