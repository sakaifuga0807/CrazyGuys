#pragma once

class BackGround;
class Player;
class GameCamera;
class Goal;
class Hammer;
class Seesaw;
class RotationGround;

class Game : public IGameObject
{
public:
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);

private:
	Player*								m_player = nullptr;						//プレイヤーのポインタをnullptrで初期化。
	BackGround*							m_backGround=nullptr;					//ステージのポインタ。
	GameCamera*							m_gameCamera=nullptr;					//カメラのポインタ。
	Goal*								m_goal=nullptr;							//ゴールのポインタ。
	std::vector<RotationGround*>		m_rotationGrounds;						//回転床の配列。
	std::vector<Hammer*>				m_hammers;								//ハンマーの配列。
	std::vector<Seesaw*>				m_seesaws;								//シーソーの配列。
	SkyCube*							m_skyCube = nullptr;					//スカイキューブ。
	SpriteRender						m_spriteRender;							//スプライトレンダー。
	FontRender							m_fontRender;							//フォントレンダー。
	Vector3								m_position=Vector3::Zero;				//座標。
	Vector3								m_skyCubePosition = Vector3::Zero;		//スカイキューブの座標。
	Vector3								m_spritePosition = Vector3::Zero;		//スプライトの位置。
	Vector3								m_fontPosition = Vector3::Zero;			//フォントの位置。
	static const int					MAX_PLAYER= 4;							//最大プレイヤー数。
	int									m_numPlayers = 0;						//プレイヤー数を0に初期化。
	float								m_skyCubeScale = 0.0f;					//スカイキューブの大きさ。
	float								m_skyCubeLuminance = 0.0f;				//スカイキューブの明るさ。
public:
	bool								m_isDelete = false;						//削除するか。
};	

