#pragma once

class BackGround;
class Player;
class GameCamera;
class Goal;
class Hammer;
class Seesaw;

class Game : public IGameObject
{
public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	Player*			m_player=nullptr;			//プレイヤーのポインタ。
	BackGround*		m_backGround=nullptr;		//ステージのポインタ。
	GameCamera*		m_gameCamera=nullptr;		//カメラのポインタ。
	Goal*			m_goal=nullptr;				//ゴールのポインタ。
	Hammer*			m_hammer=nullptr;			//ハンマーのポインタ。
	Seesaw*			m_seesaw = nullptr;			//シーソーのポインタ。
	SkyCube*		m_skyCube = nullptr;		//スカイキューブ。
	Vector3			m_position=Vector3::Zero;	//座標。
public:
	bool			m_isDelete = false;			//削除するか。
};

