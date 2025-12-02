#include "stdafx.h"
#include "Load.h"
#include "FadeManager.h"
#include "Game.h"

bool Load::Start()
{
	//背景画像。
	m_sprite.Init("Assets/sprite/Load.dds", 1920.0f, 1080.0f);
	m_sprite.Update();

	//キャラの画像。
	m_spriteCharacter.Init("Assets/sprite/Guys.dds", 200.0f, 400.0f);
	m_sprite.Update();

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

	if (m_isFinish && !FadeManager::GetInstance()->IsFadeing())
	{
		DeleteGO(this);
	}
}

void Load::Render(RenderContext& rc)
{
	m_sprite.Draw(rc);
	m_spriteCharacter.Draw(rc);
}