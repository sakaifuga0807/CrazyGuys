#include "stdafx.h"
#include "CountdownManager.h"
#include "JsonUtility.h"
#include "SoundManager.h"

#include <array>

namespace
{
	const float COUNTDOWN_TIME = 4.0f;

	const std::array<const char*, 4> SOUND_NAMES =
	{
		"Countdown3",
		"Countdown2",
		"Countdown1",
		"CountdownGO"
	};
}

bool CountdownManager::Start()
{
	//Jsonデータ読み込み。
	json configData;
	if (!JsonUtility::LoadJson("Assets/config/CountdownManager.json", configData))
	{
		return false;
	}

	auto countdownData = configData["CountdownManager"];

	//スプライト設定取得。
	auto spritePaths = countdownData["Sprites"];
	auto spriteSize = countdownData["SpriteSize"];
	auto pos = countdownData["Position"];
	auto scale = countdownData["Scale"];

	for (const auto& path : spritePaths)
	{
		auto sprite = std::make_unique<SpriteRender>();
		sprite->Init(path.get<std::string>().c_str(), spriteSize[0], spriteSize[1]);
		sprite->SetPosition({ pos[0], pos[1], pos[2] });
		sprite->SetScale({ scale[0], scale[1], scale[2] });

		m_sprites.push_back(std::move(sprite));
	}

	//各種パラメータ。
	m_displayDuration = countdownData["DisplayDuration"];
	m_startRotationTime = countdownData["StartRotationTime"];
	m_rotationSpeed = countdownData["RotationSpeed"];
	m_scaleShrinkRate = countdownData["ScaleShrinkRate"];

	//サウンド読み込み。
	SoundManager::Get().LoadFromJson("Assets/config/Sound.json");

	return true;
}

void CountdownManager::Update()
{
	if (!m_isStarted || m_isFinished)
	{
		return;
	}

	UpdateCountdown();
	UpdateRotation();

	m_sprites[m_currentIndex]->Update();
}

//----------------------------------------------------
// カウントダウン処理
//----------------------------------------------------
void CountdownManager::UpdateCountdown()
{
	int prevIndex = m_currentIndex;

	m_timer -= g_gameTime->GetFrameDeltaTime();

	if (m_timer <= 0.0f)
	{
		m_isFinished = true;
		return;
	}

	//残り時間からインデックス算出。
	m_currentIndex = static_cast<int>(COUNTDOWN_TIME - m_timer);

	if (m_currentIndex != prevIndex)
	{
		SoundManager::Get().Play(SOUND_NAMES[m_currentIndex]);

		m_timeInCurrent = 0.0f;
		m_rotation = 0.0f;
	}
}

//----------------------------------------------------
// 回転・縮小処理
//----------------------------------------------------
void CountdownManager::UpdateRotation()
{
	//GOは回転させない。
	if (m_currentIndex == 3)
	{
		return;
	}

	m_timeInCurrent += g_gameTime->GetFrameDeltaTime();

	float scale = 1.0f;

	if (m_timeInCurrent > m_startRotationTime)
	{
		float t = (m_timeInCurrent - m_startRotationTime)
			/ (m_displayDuration - m_startRotationTime);

		m_rotation = t * m_rotationSpeed;
		scale = 1.0f - t * m_scaleShrinkRate;
	}

	m_rot.SetRotationDegZ(m_rotation);
	m_sprites[m_currentIndex]->SetRotation(m_rot);
	m_sprites[m_currentIndex]->SetScale({ scale, scale, 1.0f });
}

void CountdownManager::StartCountdown()
{
	m_isStarted = true;
	m_isFinished = false;
	m_timer = COUNTDOWN_TIME;
	m_currentIndex = -1;
	m_timeInCurrent = 0.0f;
	m_rotation = 0.0f;
}

void CountdownManager::Render(RenderContext& rc)
{
	if (!m_isStarted || m_isFinished)
	{
		return;
	}

	if (m_currentIndex < m_sprites.size())
	{
		m_sprites[m_currentIndex]->Draw(rc);
	}
}
