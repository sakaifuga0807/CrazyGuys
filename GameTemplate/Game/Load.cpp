#include "stdafx.h"
#include "Load.h"
#include "FadeManager.h"
#include "Game.h"

bool Load::Start()
{
	FadeManager::GetInstance()->ForceClear();

	//背景画像。
	m_spriteBack.Init("Assets/sprite/Back.dds", 1920.0f, 1080.0f);
	m_spriteBack.Update();

	//ロードの文字。
	m_spriteLoading.Init("Assets/sprite/Loading.dds", 1920.0f, 1080.0f);
	m_spriteLoading.SetPosition({ 100.0f,-400.0f,0.0f });
	m_spriteLoading.Update();

	//キャラの画像。
	m_spriteCharacter.Init("Assets/sprite/Guys.dds", 250.0f,200.0f);
	m_spriteCharacter.Update();

	//初期化。
	m_timer = 0.0f;
	m_isFinish = false;

	return true;
}

Load::Load()
{

}

Load::~Load()
{

}

void Load::Update()
{
	//キャラクターを上下に動かす。
	m_timer += g_gameTime->GetFrameDeltaTime();

	float y = sinf(m_timer * 3.0f) * 30.0f;
	m_spriteCharacter.SetPosition({ 850.0f,-400.0f + y,0.0f });
	m_spriteCharacter.Update();

	if (m_timer >= 1.0f && !m_isFinish)
	{
		m_isFinish = true;

		NewGO<Game>(0, "game");

		FadeManager::GetInstance()->StartFadeIn(0.5f);
	}

	if (m_isFinish && !FadeManager::GetInstance()->IsFading())
	{
		DeleteGO(this);
	}
}

void Load::Render(RenderContext& rc)
{
	if (m_isFinish)
	{
		return;
	}

	m_spriteBack.Draw(rc);
	m_spriteCharacter.Draw(rc);
	m_spriteLoading.Draw(rc);
}