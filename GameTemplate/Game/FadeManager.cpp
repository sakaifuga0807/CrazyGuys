#include "stdafx.h"
#include "FadeManager.h"

bool FadeManager::Start()
{
	m_spriteRender.Init("Assets/sprite/white.dds", 1920.0f, 1080.0f);

	//初期化。
	m_alpha = 0.0f;
	m_state = FadeState::None;
	m_fadeColor = g_vec4Black;

	Vector4 c = m_fadeColor;
	c.w = m_alpha;
	m_spriteRender.SetMulColor(c);
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
	auto*instance = FindGO<FadeManager>("fademanager");
	if (!instance)
	{
		instance = NewGO<FadeManager>(10, "fademanager");
		instance->Start();
	}
	return instance;
}

void FadeManager::StartFadeOut(float duration, std::function<void()>onComplete, const Vector4& color)
{
	m_state = FadeState::FadeOut;
	m_alpha = 0.0f;
	m_targetAlpha = 1.0f;
	m_fadeSpeed = (m_targetAlpha - m_alpha) / duration;
	m_fadeColor = color;
	m_onComplete = onComplete;

	Vector4 c = m_fadeColor;
	c.w = m_alpha;
	m_spriteRender.SetMulColor(c);
}

void FadeManager::StartFadeIn(float duration, std::function<void()>onComplete, const Vector4& color)
{
	m_state = FadeState::FadeIn;
	m_alpha = 1.0f;
	m_targetAlpha = 0.0f;
	m_fadeSpeed = (m_targetAlpha - m_alpha) / duration;
	m_fadeColor = color;
	m_onComplete = onComplete;

	Vector4 c = m_fadeColor;
	c.w = m_alpha;
	m_spriteRender.SetMulColor(c);
}

void FadeManager::Update()
{
	if (m_state == FadeState::None)
	{
		return;
	}

	//フェード。
	m_alpha += m_fadeSpeed * g_gameTime->GetFrameDeltaTime();
	
	//フェード完了判定。
	if ((m_fadeSpeed > 0 && m_alpha >= m_targetAlpha) || (m_fadeSpeed < 0 && m_alpha <= m_targetAlpha))
	{
		m_alpha = m_targetAlpha;
		m_state = FadeState::None;

		if (m_onComplete)
		{
			m_onComplete();
			m_onComplete = nullptr;
		}
	}

	Vector4 color = m_fadeColor;
	color.w = m_alpha;
	m_spriteRender.SetMulColor(color);
	m_spriteRender.Update();
}

void FadeManager::Render(RenderContext&rc)
{
	//α値が0以下だったら描画しない。
	if (m_alpha <= 0.0f)
	{
		return;
	}

	m_spriteRender.Draw(rc);
}