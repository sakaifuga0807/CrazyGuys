#pragma once


class BackGround;
class Player;
class GameCamera;
class Goal;
class Hammer;
class Seesaw;
class Axe;
class RotationGround;
class CountdownManager;

class Game : public IGameObject
{
public:

	//ゲームフェーズの列挙型。
	enum EnGamePhase
	{
		enGamePhase_CameraDemo,//カメラデモ
		enGamePhase_Countdown,//カウントダウン
		enGamePhase_Playing,//プレイ中
	};

	Game();
	~Game();
	bool Start();
	void Update();
	void MakePlayers();
	void MakeRotationGrounds();
	void MakeHammers();
	void MakeSeesaw();
	void MakeAxes();
	void GameConfig();
	void ShowRoundOver();
	void OnPlayerGoal(Player*winner);
	void GoToResult(Player* winner);
	void UpdateCameraDemo();
	void Render(RenderContext& rc);

private:
	BackGround*							m_backGround=nullptr;					//ステージのポインタ。
	GameCamera*							m_gameCamera=nullptr;					//カメラのポインタ。
	Goal*								m_goal=nullptr;							//ゴールのポインタ。
	nsK2Engine::SkyCube*							m_skyCube = nullptr;					//スカイキューブ。
	Player*								m_winner = nullptr;						//勝者のプレイヤー。
	CountdownManager*					m_countdown = nullptr;					//カウントダウンのポインタ。
	EnGamePhase							m_gamePhase = enGamePhase_CameraDemo;	//ゲームフェーズ。
	std::vector<Player*>				m_players;								//プレイヤーの配列。
	std::vector<RotationGround*>		m_rotationGrounds;						//回転床の配列。
	std::vector<Hammer*>				m_hammers;								//ハンマーの配列。
	std::vector<Seesaw*>				m_seesaws;								//シーソーの配列。
	std::vector<Axe*>					m_axes;									//斧の配列。
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
	float								m_phaseTimer = 0.0f;					//フェーズタイマー。
	bool								m_isCountdownFinished = false;			//カウントダウンが終わったか。
	bool								m_showRoundOver = false;				//ラウンドオーバーかどうか。
	bool								m_isGoal = false;						//ゴールしたか。
	bool								m_isResultCreated = false;				//リザルト画面が作成されたか。
public:
	bool								m_isDelete = false;						//削除するか。
};

