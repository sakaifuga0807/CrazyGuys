#include "stdafx.h"
#include "Title.h"
#include "SelectPlayers.h"
#include "Game.h"

bool Title::Start()
{
	//スプライトを読み込む。
	m_spriteRender.Init("Assets/sprite/TitleDemo.dds", 1920.0f, 1080.0f);
	m_spriteRender.Update();

	return true;
}

Title::Title()
{

}

Title::~Title()
{

}

void Title::Update()
{
	//ゲームへ移行させる。
	if (g_pad[0]->IsTriggerAnyKey())
	{
		if (FindGO<Game>("game") == nullptr)
		{
			NewGO<Game>(0, "game");
		}
		DeleteGO(this);
	}
}

void Title::Render(RenderContext& rc)
{
	m_spriteRender.Draw(rc);
}