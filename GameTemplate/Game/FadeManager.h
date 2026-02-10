#pragma once

class FadeManager : public IGameObject
{
public:
	//フェードの状態を表す列挙型。
	enum class FadeState
	{
		None,		//何もしない。
		FadeIn,		//フェードイン。
		FadeOut		//フェードアウト。
	};

	FadeManager();
	~FadeManager();

	//初期化処理。
	bool Start();

	//更新処理。
	void Update();

	//描画処理。
	void Render(RenderContext& rc);

	//グローバルインスタンスを取得する。
	static FadeManager* GetInstance();

	//フェードアウトを開始する。
	void StartFadeOut(
		float duration,
		std::function<void()> onComplete = nullptr,
		const Vector4& color = g_vec4Black
	);

	//フェードインを開始する。
	void StartFadeIn(
		float duration,
		std::function<void()> onComplete = nullptr,
		const Vector4& color = g_vec4Black
	);

	//フェード状態を強制的に解除する。
	void ForceClear();

	//フェード中かどうかを取得する。
	bool IsFading() const
	{
		return m_state != FadeState::None;
	}

	//現在のアルファ値を取得する。
	float GetAlpha() const
	{
		return m_alpha;
	}

private:
	std::function<void()> m_onComplete = nullptr;//フェード完了時のコールバック。
	FadeState		m_state = FadeState::None;		//現在のフェード状態。
	float			m_alpha = 0.0f;					//現在のアルファ値。
	float			m_fadeSpeed = 0.0f;				//フェード速度。
	float			m_targetAlpha = 0.0f;			//目標のアルファ値。
	Vector4			m_fadeColor = g_vec4Black;		//フェードカラー。
	SpriteRender	m_spriteRender;					//フェード用スプライト。
};
