#include "stdafx.h"
#include "Title.h"
#include "Game.h"
#include "GameSettings.h"
#include "SoundManager.h"

bool Title::Start()
{
	//スプライトを読み込む。
	m_spriteRender.Init("Assets/sprite/Title.dds", 1920.0f, 1080.0f);
	m_spriteRender.Update();

	m_selectPlayerCount = 1;

	//文字の初期化。
	m_fontRender.SetColor(g_vec4Black);
	m_fontRender.SetPosition(0.0f,-400.0f,0.0f);
	m_fontRender.SetScale(2.0f);
	
	//ロード中の文字の初期化。
	//m_fontLoading.SetText(L"NowLoading");
	m_fontLoading.SetColor(g_vec4White);
	m_fontLoading.SetPosition(300.0f, -400.0f, 0.0f);
	m_fontLoading.SetScale(2.0f);
	m_fontLoading.SetText(L"NowLoading");

	//音などの情報をロードする。
	SoundManager::Get().LoadFromJson("Assets/config/Sound.json");
	//再生。
	SoundManager::Get().Play("MainMenu");
	

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
		//
		if (g_pad[0]->IsTrigger(enButtonA))
		{
			m_isStart = true;
			m_dontCount = 0;
			m_dotTimer = 0.0f;

			SoundManager::Get().Play("Enter");
			SoundManager::Get().StopBGM();
		}
		return;
	}

	//アニメーション。
	m_dotTimer += 0.005f;
	if (m_dotTimer >= 1.0)
	{
		m_dotTimer = 0.0f;
		m_dontCount++;

		if (m_dontCount > 3)
		{
			m_dontCount = 0;
		}

		//ドットの数を変更。
		wchar_t buf[64];
		swprintf_s(buf, L"NowLoading%.*s", m_dontCount, L"...");

		m_fontLoading.SetText(buf);
	}

	if (m_dontCount == 3)
	{
		NewGO<Game>(0, "Game");
		DeleteGO(this);
	}

	/*//Aボタンを押すとプレイヤーの数を決定してGameへ移行。
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
	}*/


	//マルチ用。
	//左ボタンを押すとプレイヤーの数を減らす。
	/*if (g_pad[0]->IsTrigger(enButtonLeft))
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
	m_fontRender.SetText(buf);*/
}

void Title::Render(RenderContext& rc)
{
	m_spriteRender.Draw(rc);
	if (m_isStart)
	{
		m_fontLoading.Draw(rc);
	}
	//m_fontRender.Draw(rc);
}