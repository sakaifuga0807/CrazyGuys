#include "stdafx.h"
#include "SoundManager.h"
#include "JsonUtility.h"

SoundManager& SoundManager::Get()
{
	static SoundManager instance;
	return instance;
}

bool SoundManager::LoadFromJson(const char* jsonPath)
{
	json data;
	if (!JsonUtility::LoadJson(jsonPath, data))
	{
		return false;
	}

	auto sounds = data["Sounds"];
	for (auto& s : sounds)
	{
		SoundData sd;
		sd.id = s["ID"];
		sd.name = s["Name"];
		sd.path = s["Path"];
		sd.volume = s["Volume"];
		sd.loop = s["Loop"];

		//サウンド登録。
		g_soundEngine->ResistWaveFileBank(sd.id, sd.path.c_str());

		//名前で呼び出されるようにする。
		m_soundTable[sd.name] = sd;
	}

	return true;
}

void SoundManager::Play(const std::string& name)
{
	const auto& sd = m_soundTable[name];

	//BGMは別管理。	
	if (sd.loop)
	{
		//1種類のBGMしか流せないようにする。
		if (m_bgm)
		{
			m_bgm->Stop();
			DeleteGO(m_bgm);
		}

		m_bgm = NewGO<SoundSource>(0);
		m_bgm->Init(sd.id);
		m_bgm->SetVolume(sd.volume);
		m_bgm->Play(true);
		return;
	}

	//SEは複数管理
	auto se = NewGO<SoundSource>(0);
	se->Init(sd.id);
	se->SetVolume(sd.volume);
	se->Play(false);
}

void SoundManager::StopBGM()
{
	if (m_bgm)
	{
		m_bgm->Stop();
		DeleteGO(m_bgm);
		m_bgm = nullptr;
	}
}