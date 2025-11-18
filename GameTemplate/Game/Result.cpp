#include "stdafx.h"
#include "Result.h"
#include "Title.h"
#include "SoundManager.h"

bool Result::Start()
{
	//モデルを初期化。
	m_spriteRender.Init("Assets/sprite/Result.dds", 1920.0f,1080.0f);
	m_spriteRender.Update();
	
	//音を読み込む。
	SoundManager::Get().LoadFromJson("Assets/config/Sound.json");
	SoundManager::Get().Play("Result");

	return true;
}

Result::Result()
{

}

Result::~Result()
{

}

void Result::Update()
{
	if (m_isEnd)
	{
		return;
	}

	//Aボタンが押されたらタイトルへ。
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		//ゲームが終わった。
		m_isEnd = true;

		//決定音を鳴らす。
		SoundManager::Get().Play("Enter");
		SoundManager::Get().StopBGM();

		//タイトルへ。
		m_title = NewGO<Title>(0, "title");
		DeleteGO(this);
	}
}

void Result::Render(RenderContext& rc)
{
	m_spriteRender.Draw(rc);
}