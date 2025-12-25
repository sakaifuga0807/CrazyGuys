#include "stdafx.h"
#include "CountdownManager.h"
#include "JsonUtility.h"
#include "SoundManager.h"


#include <array>  

bool CountdownManager::Start()  
{  
	//Jsonデータを格納する変数。
	json configData;
	if (!JsonUtility::LoadJson("Assets/config/CountdownManager.json", configData))
	{
		return false;
	}

	auto countdownData = configData["CountdownManager"];

	
	//スプライトの読み込み。
	auto spritePaths = countdownData["Sprites"];
	auto spriteSize = countdownData["SpriteSize"];
	auto pos = countdownData["Position"];
	auto scale = countdownData["Scale"];

	for (const auto& path : spritePaths)
	{
		auto sprite = std::make_unique<SpriteRender>();
		sprite->Init(path.get<std::string>().c_str(), spriteSize[0], spriteSize[1]);
		sprite->SetPosition({ pos[0],pos[1],pos[2] });
		sprite->SetScale({ scale[0],scale[1],scale[2] });

		m_sprites.push_back(std::move(sprite));
	}

	//1枚当たりの表示時間。
	m_displayDuration = countdownData["DisplayDuration"];
	//回転を始めるタイミング。
	m_startRotationTime = countdownData["StartRotationTime"];
	//回転速度。
	m_rotationSpeed = countdownData["RotationSpeed"];
	//縮小率。
	m_scaleShrinkRate = countdownData["ScaleShrinkRate"];

	//サウンドの読み込み。
	SoundManager::Get().LoadFromJson("Assets/config/Sound.json");

    return true;  
}

void CountdownManager::Update()
{
	if (!m_isStarted)
	{
		return;
	}

	if (m_isFinished)
	{
		return;
	}

	//カウントダウン。
	Countdown();

	//回転。
	Rotation();

	m_sprites[m_currentIndex]->Update();
}

//カウントダウン。
void CountdownManager::Countdown()
{
	int prevIndex = m_currentIndex;
	//タイマーを減らす。
	m_timer -= g_gameTime->GetFrameDeltaTime();

	//各画像の表示タイミングを設定。
	if (m_timer<=4.0f&&m_timer > 3.0f)
	{
		m_currentIndex = 0;
	}
	else if (m_timer<=3.0f&&m_timer > 2.0f)
	{
		m_currentIndex = 1;
	}
	else if (m_timer<=2.0f&&m_timer > 1.0f)
	{
		m_currentIndex = 2;
	}
	else if (m_timer <= 1.0f && m_timer > 0.0f)
	{
		m_currentIndex = 3;
	}
	else
	{
		m_isFinished = true;
		return;
	}

	if (m_currentIndex != prevIndex)
	{
		//サウンド再生。
		if (m_currentIndex == 0)SoundManager::Get().Play("Countdown3");
		if (m_currentIndex == 1)SoundManager::Get().Play("Countdown2");
		if (m_currentIndex == 2)SoundManager::Get().Play("Countdown1");
		if (m_currentIndex == 3)SoundManager::Get().Play("CountdownGO");

		m_timeInCurrent = 0.0f;
		m_rotation = 0.0f;
	}
}

//回転。
void CountdownManager::Rotation()
{
	if (m_currentIndex == 3)
	{
		return;
	}

	float scale = 1.0f;
	//表示してからどれだけ経過したかを計算。
	m_timeInCurrent += g_gameTime->GetFrameDeltaTime();

	//消える直前に回転させる。
	if (m_timeInCurrent > m_startRotationTime)
	{
		float t = (m_timeInCurrent - m_startRotationTime) / (m_displayDuration - m_startRotationTime);
		//一回転させる。
		m_rotation = t * m_rotationSpeed;
		Math::DegToRad(m_rotation);
		//小さくなって消える。
		scale = 1.0f - t * m_scaleShrinkRate;
	}

	m_rot.SetRotationDegZ(m_rotation);
	m_sprites[m_currentIndex]->SetRotation(m_rot);
	m_sprites[m_currentIndex]->SetScale({ scale,scale,1.0f });
}

void CountdownManager::StartCountdown()
{
	m_isStarted = true;
	m_isFinished = false;
	m_timer = 4.0f;
	m_currentIndex = -1;
	m_timeInCurrent = 0.0f;
	m_rotation = 0.0f;
}

void CountdownManager::Render(RenderContext& rc)
{
	if (!m_isStarted)
	{
		return;
	}

	if (!m_isFinished && m_currentIndex < m_sprites.size())
	{
		m_sprites[m_currentIndex]->Draw(rc);
	}
}