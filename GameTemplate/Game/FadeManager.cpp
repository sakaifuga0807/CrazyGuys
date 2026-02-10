#include "stdafx.h"
#include "FadeManager.h"

bool FadeManager::Start()
{
	//フェード用スプライトを初期化する。
	m_spriteRender.Init("Assets/sprite/white.dds", 1920.0f, 1080.0f);

	//フェード初期状態を設定する。
	m_alpha = 0.0f;
	m_state = FadeState::None;
	m_fadeColor = g_vec4Black;

	//初期カラーを反映する。
	Vector4 color = m_fadeColor;
	color.w = m_alpha;
	m_spriteRender.SetMulColor(color);
	m_spriteRender.Update();

	return true;
}

FadeManager::FadeManager()
{
}

FadeManager::~FadeManager()
{
}

FadeManager* FadeManager::GetInstance()
{
	//既存のFadeManagerを取得する。
	auto* instance = FindGO<FadeManager>("fademanager");

	//存在しなければ生成する。
	if (!instance)
	{
		instance = NewGO<FadeManager>(10, "fademanager");
		instance->Start();
	}

	return instance;
}

void FadeManager::StartFadeOut(float duration, std::function<void()> onComplete, const Vector4& color)
{
	//フェードアウト状態に設定する。
	m_state = FadeState::FadeOut;
	m_alpha = 0.0f;
	m_targetAlpha = 1.0f;
	m_fadeSpeed = (m_targetAlpha - m_alpha) / duration;
	m_fadeColor = color;
	m_onComplete = onComplete;

	//初期カラーを反映する。
	Vector4 c = m_fadeColor;
	c.w = m_alpha;
	m_spriteRender.SetMulColor(c);
}

void FadeManager::StartFadeIn(float duration, std::function<void()> onComplete, const Vector4& color)
{
	//フェードイン状態に設定する。
	m_state = FadeState::FadeIn;
	m_alpha = 1.0f;
	m_targetAlpha = 0.0f;
	m_fadeSpeed = (m_targetAlpha - m_alpha) / duration;
	m_fadeColor = color;
	m_onComplete = onComplete;

	//初期カラーを反映する。
	Vector4 c = m_fadeColor;
	c.w = m_alpha;
	m_spriteRender.SetMulColor(c);
}

void FadeManager::Update()
{
	//フェード中でなければ処理しない。
	if (m_state == FadeState::None)
	{
		return;
	}

	//アルファ値を更新する。
	m_alpha += m_fadeSpeed * g_gameTime->GetFrameDeltaTime();

	//フェード完了判定を行う。
	if ((m_fadeSpeed > 0.0f && m_alpha >= m_targetAlpha) ||
		(m_fadeSpeed < 0.0f && m_alpha <= m_targetAlpha))
	{
		m_alpha = m_targetAlpha;
		m_state = FadeState::None;

		//完了時コールバックを実行する。
		if (m_onComplete)
		{
			m_onComplete();
			m_onComplete = nullptr;
		}
	}

	//スプライトにカラーを反映する。
	Vector4 color = m_fadeColor;
	color.w = m_alpha;
	m_spriteRender.SetMulColor(color);
	m_spriteRender.Update();
}

void FadeManager::ForceClear()
{
	//フェード状態を強制的に解除する。
	m_alpha = 0.0f;
	m_state = FadeState::None;

	//完全に透明にする。
	Vector4 c = m_fadeColor;
	c.w = 0.0f;
	m_spriteRender.SetMulColor(c);
	m_spriteRender.Update();
}

void FadeManager::Render(RenderContext& rc)
{
	//完全に透明なら描画しない。
	if (m_alpha <= 0.0f)
	{
		return;
	}

	m_spriteRender.Draw(rc);
}
