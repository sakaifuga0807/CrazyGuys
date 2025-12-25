#include "stdafx.h"
#include "Title.h"
#include "Game.h"
#include "GameSettings.h"
#include "SoundManager.h"
#include "FadeManager.h"
#include "Load.h"

bool Title::Start()
{
	m_isStart = false;

	//BGMを強制的に止める。
	SoundManager::Get().StopBGM();

	//スプライトを読み込む。
	m_spriteRender.Init("Assets/sprite/Title.dds", 1920.0f, 1080.0f);
	m_spriteRender.Update();

	m_selectPlayerCount = 1;

	//音などの情報をロードする。
	SoundManager::Get().LoadFromJson("Assets/config/Sound.json");
	//再生。
	SoundManager::Get().Play("MainMenu");
	
	FadeManager::GetInstance()->ForceClear();

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
	if (!m_isStart)
	{
		//Aボタンを押すとロード画面へ。
		if (g_pad[0]->IsTrigger(enButtonA))
		{
			m_isStart = true;

			SoundManager::Get().Play("Enter");
			SoundManager::Get().StopBGM();

			FadeManager::GetInstance()->StartFadeOut(0.5f,nullptr,g_vec4Black);
		}
		return;
	}

	if (m_isStart && !FadeManager::GetInstance()->IsFadeing())
	{
		NewGO<Load>(0, "load");
		DeleteGO(this);
	}

	//Aボタンを押すとプレイヤーの数を決定してGameへ移行。
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		//始まったフラグを立てる。
		m_isStart = true;

		//決定音を鳴らす。
		SoundManager::Get().Play("Enter");

		//音を止める。
		SoundManager::Get().StopBGM();

		//プレイヤーを数える。
		GameSettings::PlayerCount = m_selectPlayerCount;

		//Gameを生成。
		NewGO<Game>(0, "Game");

		DeleteGO(this);
	}


	/*//マルチ用。
	//左ボタンを押すとプレイヤーの数を減らす。
	if (g_pad[0]->IsTrigger(enButtonLeft))
	{
		m_selectPlayerCount--;
		if (m_selectPlayerCount < 1)
		{
			m_selectPlayerCount = 4;
		}
	}

	//右ボタンを押すとプレイヤーの数を増やす。
	if (g_pad[0]->IsTrigger(enButtonRight))
	{
		m_selectPlayerCount++;
		if (m_selectPlayerCount > 4)
		{
			m_selectPlayerCount = 1;
		}
	}

	//プレイ人数を表示。
	wchar_t buf[64];
	swprintf_s(buf, L"%dP", m_selectPlayerCount);
	//m_fontRender.SetText(buf);*/
}

void Title::Render(RenderContext& rc)
{
	m_spriteRender.Draw(rc);

}