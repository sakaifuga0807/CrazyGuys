#pragma once

class BackGround;
class Player;
class GameCamera;
class Goal;


class Game : public IGameObject
{
public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	Player*			m_player;			//プレイヤー。
	BackGround*		m_backGround;		//ステージ。
	GameCamera*		m_gameCamera;		//カメラ。
	Goal*			m_goal;				//ゴール。
public:
	bool m_isDelete = false;			//削除するか。
};

