#include "stdafx.h"
#include "Result.h"
#include "Title.h"
#include "SoundManager.h"
#include "Game.h"
#include "Player.h"
#include "graphics/effect/EffectEmitter.h"

bool Result::Start()
{
	//スカイキューブを生成。
	m_skyCube = NewGO<nsK2Engine::SkyCube>(0, "skycube");
	m_skyCube->SetType(enSkyCubeType_DayToon_3);
	m_skyCube->SetScale(500.0f);
	m_skyCube->SetLuminance(1.0f);

	//背景画像を読み込む。
	m_spriteRender.Init("Assets/sprite/Winner.dds", 1920.0f,1080.0f);
	m_spriteRender.SetPosition({ 0.0f, 0.0f, 0.0f });
	m_spriteRender.Update();

	Quaternion rot;
	rot.SetRotationDeg(Vector3::AxisY,180.0f);
	m_modelRender.SetRotation(rot);

	//カメラを生成。
	g_camera3D->SetPosition({ 0.0f,50.0f,-250.0f });
	g_camera3D->SetTarget({ 0.0f,100.0f,100.0f });
	g_camera3D->Update();

	if (!m_winnerModelPath.empty())
	{
		//勝利アニメーション。
		m_victoryClip.Load("Assets/animData/Victory.tka");
		m_victoryClip.SetLoopFlag(true);

		//モデルを読み込む。
		m_modelRender.Init(m_winnerModelPath.c_str(), &m_victoryClip,1);
		//位置調整。
		m_modelRender.SetPosition({ 0.0f, -35.0f, 100.0f });
		m_modelRender.PlayAnimation(0);
	}

	//音を読み込む。
	SoundManager::Get().LoadFromJson("Assets/config/Sound.json");
	SoundManager::Get().Play("Result");

	//エフェクトを読み込む。
	//EffectEngine::GetInstance()->ResistEffect(1, u"Assets/effect/hit.efk");

	//インスタンスを検索。
	m_game = FindGO<Game>("game");

	return true;
}

Result::Result()
{

}

Result::~Result()
{
	DeleteGO(m_skyCube);
}

void Result::Update()
{
	if (m_isEnd)
	{
		return;
	}

	m_modelRender.Update();

	/*m_effectEmitter = NewGO<EffectEmitter>(0);
	m_effectEmitter->Init(1);
	m_effectEmitter->SetPosition({ 0.0f, -35.0f, 100.0f });
	m_effectEmitter->SetScale({ 1.0f,1.0f,1.0f });
	m_effectEmitter->Play();*/

	//Aボタンが押されたらタイトルへ。
	if (g_pad[0]->IsTrigger(enButtonA))
	{
		//ゲームが終わった。
		m_isEnd = true;

		SoundManager::Get().StopBGM();

		//決定音を鳴らす。
		SoundManager::Get().Play("Enter");

		//タイトルへ。
		m_title = NewGO<Title>(0, "title");

		DeleteGO(this);
	}
}

void Result::Render(RenderContext& rc)
{
	m_spriteRender.Draw(rc);

	if (!m_winnerModelPath.empty())
	{
		m_modelRender.Draw(rc);
	}
}