#pragma once


class BackGround;
class Player;
class GameCamera;
class Goal;
class Hammer;
class Seesaw;
class RotationGround;
class CountdownManager;

class Game : public IGameObject
{
public:
	Game();
	~Game();
	bool Start();
	void Update();
	void MakePlayers();
	void MakeRotationGrounds();
	void MakeHammers();
	void MakeSeesaw();
	void GameConfig();
	void ShowRoundOver();
	void OnPlayerGoal();
	void Render(RenderContext& rc);

private:
	BackGround*							m_backGround=nullptr;					//ステージのポインタ。
	GameCamera*							m_gameCamera=nullptr;					//カメラのポインタ。
	Goal*								m_goal=nullptr;							//ゴールのポインタ。
	SkyCube*							m_skyCube = nullptr;					//スカイキューブ。
	CountdownManager*					m_countdown = nullptr;					//カウントダウンのポインタ。
	std::vector<Player*>				m_players;								//プレイヤーの配列。
	std::vector<RotationGround*>		m_rotationGrounds;						//回転床の配列。
	std::vector<Hammer*>				m_hammers;								//ハンマーの配列。
	std::vector<Seesaw*>				m_seesaws;								//シーソーの配列。
	SpriteRender						m_spriteRender;							//スプライトレンダー。
	SpriteRender						m_roundOverSprite;						//ラウンドオーバーの画像。
	FontRender							m_fontRender;							//フォントレンダー。
	FontRender							m_debugFont;
	Vector3								m_position=Vector3::Zero;				//座標。
	Vector3								m_skyCubePosition = Vector3::Zero;		//スカイキューブの座標。
	Vector3								m_spritePosition = Vector3::Zero;		//スプライトの位置。
	Vector3								m_fontPosition = Vector3::Zero;			//フォントの位置。
	static const int					MAX_PLAYER= 4;							//最大プレイヤー数。
	int									m_numPlayers = 0;						//プレイヤー数を0に初期化。
	float								m_skyCubeScale = 0.0f;					//スカイキューブの大きさ。
	float								m_skyCubeLuminance = 0.0f;				//スカイキューブの明るさ。
	float								m_goalTimer = 0.0f;						//ゴールタイマー。
	bool								m_isCountdownFinished = false;			//カウントダウンが終わったか。
	bool								m_showRoundOver = false;				//ラウンドオーバーかどうか。
	bool								m_isGoal = false;						//ゴールしたか。
public:
	bool								m_isDelete = false;						//削除するか。
};	

