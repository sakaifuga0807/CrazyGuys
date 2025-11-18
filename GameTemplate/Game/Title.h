#pragma once

class SelectPlayer;
class Game;

class Title:public IGameObject
{
public:
	bool Start();
	Title();
	~Title();
	void Update();
	void Render(RenderContext& rc);

private:
	//メンバ変数。
	Game*				m_game;						//ゲームクラスのポインタ。
	SpriteRender		m_spriteRender;				//スプライトレンダー。
	FontRender			m_fontLoading;				//ロードのフォント。
	FontRender			m_fontRender;				//フォントレンダー。
	int					m_selectPlayerCount = 0;	//選ばれたプレイヤーの数。
	int					m_dontCount = 0;			//点の数。
	float				m_dotTimer = 0.0f;			//点のタイマー。
	bool				m_isStart = false;			//スタートしてるか。
};

