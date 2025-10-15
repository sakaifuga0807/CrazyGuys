#pragma once

class Title;

class SelectPlayers:public IGameObject
{
public:

	enum  EnSelectPlayer
	{
		enSolo,
		enDuo,
		enTrio,
		enSpuad
	};

	bool Start();
	SelectPlayers();
	~SelectPlayers();
	void Update();
	void Render(RenderContext& rc);

private:
	//メンバ変数。
	//SpriteRender m_spriteRender;//スプライトレンダー。
	EnSelectPlayer m_enSelectPlayer;//選択したプレイヤー人数。
};

