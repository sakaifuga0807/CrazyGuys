#pragma once

class SelectPlayer;
class Game;

class Title:public IGameObject
{
	enum EnTitleState
	{
		enTitleState_SelectMenu,			//タイトルメニュー選択。
		enTitleState_SelectPlayerCount,		//プレイヤー数選択。
		enTitleState_HowToPlay,				//遊び方説明。
	};

public:
	bool Start();
	Title();
	~Title();
	void Update();
	void Render(RenderContext& rc);

private:
	//メンバ変数。
	EnTitleState		m_state = enTitleState_SelectMenu;		//タイトルの状態。
	Game*				m_game;									//ゲームクラスのポインタ。
	SpriteRender		m_spriteRender;							//スプライトレンダー。
	int					m_selectPlayerCount = 0;				//選ばれたプレイヤーの数。
	bool				m_isStart = false;						//スタートしてるか。
};

