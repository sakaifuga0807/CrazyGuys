#pragma once
class CountdownManager:public IGameObject
{
public:
	bool Start();
	void Update();
	void Countdown();
	void Rotation();
	void Render(RenderContext& rc);

	bool IsFinished()const
	{
		return m_isFinished;
	}

private:
	std::vector<std::unique_ptr<SpriteRender>> m_sprites;			//スプライトの配列。
	Quaternion		m_rot;
	float			m_timer = 0.0f;									//カウントダウン時間。
	float			m_displayDuration = 0.0f;						//表示時間。
	float			m_timeInCurrent = 0.0f;							//どれだけ経過したか。
	float			m_rotation=0.0f;								//回転。
	float			m_startRotationTime=0.0f;						//回転を始める時間。
	float			m_rotationSpeed = 0.0f;							//回転速度。
	float			m_scaleShrinkRate = 0.0f;						//縮小率。
	bool			m_isFinished = false;							//終了したか。
	int				m_currentIndex = 0;								//現在のインデックス。
};

