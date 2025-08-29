#include "stdafx.h"
#include "Game.h"
#include "BackGround.h"
#include "Player.h"
#include "GameCamera.h"

bool Game::Start()
{
	//ステージを生成。
	m_backGround=NewGO<BackGround>(0,"background");
	//プレイヤーを生成。
	m_player=NewGO<Player>(0,"player");
	//カメラを生成。
	m_gameCamera = NewGO<GameCamera>(0, "gamecamera");
	return true;
}

Game::Game()
{

}

Game::~Game()
{

}

void Game::Update()
{
}

void Game::Render(RenderContext& rc)
{
}