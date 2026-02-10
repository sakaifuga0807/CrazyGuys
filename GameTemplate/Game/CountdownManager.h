#pragma once

#include <vector>
#include <memory>

class CountdownManager : public IGameObject
{
public:
	//初期化処理。
	bool Start();

	// 毎フレーム更新処理。
	void Update();

	//カウントダウン開始。
	void StartCountdown();

	//描画処理。
	void Render(RenderContext& rc);

	//カウントダウンが終了しているか。
	bool IsFinished() const
	{
		return m_isFinished;
	}

private:
	//残り時間をもとに表示スプライトと状態を更新する。
	void UpdateCountdown();

	//回転・縮小演出を更新する。
	void UpdateRotation();

private:
	//カウントダウン用スプライト配列。
	std::vector<std::unique_ptr<SpriteRender>> m_sprites;
	Quaternion	m_rot;							//回転用クォータニオン。
	float		m_timer = 0.0f;					//残りカウントダウン時間。
	float		m_displayDuration = 0.0f;		//1枚あたりの表示時間。
	float		m_timeInCurrent = 0.0f;			//現在の画像が表示されてからの経過時間。
	float		m_rotation = 0.0f;				//現在の回転角度（度）。
	float		m_startRotationTime = 0.0f;		//回転を開始する時間。
	float		m_rotationSpeed = 0.0f;			//回転速度。
	float		m_scaleShrinkRate = 0.0f;		//縮小率。
	int			m_currentIndex = -1;			//現在表示しているスプライトのインデックス。
	bool		m_isStarted = false;			//カウントダウンが開始されているか。
	bool		m_isFinished = false;			//カウントダウンが終了したか。
};
