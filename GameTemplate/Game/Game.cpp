#include "stdafx.h"
#include "Game.h"
#include "BackGround.h"
#include "Player.h"
#include "GameCamera.h"

bool Game::Start()
{
	//�X�e�[�W�𐶐��B
	m_backGround=NewGO<BackGround>(0,"background");
	//�v���C���[�𐶐��B
	m_player=NewGO<Player>(0,"player");
	//�J�����𐶐��B
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