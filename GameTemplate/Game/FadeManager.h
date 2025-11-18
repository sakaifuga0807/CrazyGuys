#pragma once

class FadeManager:public IGameObject
{
public:
	//フェードの状態を表す列挙型。
	enum class FadeState
	{
		None,		//フェードなし。
		FadeIn,		//フェードイン。
		FadeOut		//フェードアウト。
	};

	bool Start();
	FadeManager();
	~FadeManager();
	void Update();

	//グローバルインスタンスを取得。
	static FadeManager* GetInstance();

	//フェードアウトを開始。
	void StartFadeOut(float duration, std::function<void()>onComplete = nullptr, const Vector4& color = g_vec4Black);

	//フェードインを開始。
	void StartFadeIn(float duration, std::function<void()>onCoplete = nullptr, const Vector4& color = g_vec4Black);

	//フェード中かどうか。
	bool IsFadeing()const
	{
		return m_state != FadeState::None;
	}

	//現在のα値。
	float GetAlph() const
	{
		return m_alpha;
	}

	void Render(RenderContext& rc);


	FadeState m_state = FadeState::None;
	float m_alpha = 0.0f;
	float m_fadeSpeed = 0.0f;
	float m_targetAlpha = 0.0f;
	Vector4 m_fadeColor = g_vec4Black;
	std::function<void()>m_onComplete = nullptr;
	SpriteRender m_spriteRender;
};

