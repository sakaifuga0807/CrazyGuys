#pragma once

#include "sound/SoundSource.h"
#include <unordered_map>
#include <string>

class SoundManager:public IGameObject
{
public:
	static SoundManager& Get();

	//読み込み。
	bool LoadFromJson(const char* jsonPath);
	//再生。
	void Play(const std::string& name);
	//停止。
	void StopBGM();

private:
	SoundManager() {}

	struct  SoundData
	{
		int id;
		std::string name;
		std::string path;
		float volume = 1.0f;
		bool loop = false;
	};
	std::unordered_map<std::string, SoundData> m_soundTable;
	SoundSource* m_bgm = nullptr;		//BGM専用。

};

