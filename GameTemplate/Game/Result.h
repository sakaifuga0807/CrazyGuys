#pragma once

class Title;
class Player;

#include "Player.h"

class Result:public IGameObject
{
public:
	bool Start();
	Result();
	~Result();
	void Update();
	void Render(RenderContext& rc);

	//ŸÒ‚ğó‚¯æ‚é‚½‚ß‚ÌŠÖ”B
	void SetWinnerModelPath(const char* path)
	{
		if (path)
		{
			m_winnerModelPath = path;
		}
	}


private:
	//ƒƒ“ƒo•Ï”B
	nsK2Engine::SkyCube* m_skyCube;
	std::string m_winnerModelPath;
	AnimationClip m_victoryClip;
	SpriteRender m_spriteRender;
	EffectEmitter* m_effectEmitter;
	ModelRender m_modelRender;
	Title* m_title;
	Game* m_game;

	float m_angle = 0.0f;
public:
	bool m_isEnd = false;
};