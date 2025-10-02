#pragma once

class Title;

class SelectPlayers:public IGameObject
{
public:

	enum  EnSelectPlayer
	{

	};

	bool Start();
	SelectPlayers();
	~SelectPlayers();
	void Update();
	void Render(RenderContext& rc);

private:
	//メンバ変数。
	//SpriteRender m_spriteRender;//スプライトレンダー。
};

