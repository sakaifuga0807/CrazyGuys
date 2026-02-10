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
class MultiViewRender;
class PlayerManager;
class StageObjectFactory;

class Game : public IGameObject
{
public:

	//ゲームフェーズの列挙型。
	enum EnGamePhase
	{
		enGamePhase_CameraDemo,//カメラデモ。
		enGamePhase_Countdown,//カウントダウン。
		enGamePhase_Playing,//プレイ中。
	};

	Game();
	~Game();
	bool Start();
	void Update();
	void GameConfig();
	void ShowRoundOver();
	void MakePlayers();
	void OnPlayerGoal(Player*winner);
	void GoToResult(Player* winner);
	void UpdateCameraDemo();
	void Render(RenderContext& rc);

	static bool IsReady()
	{
		return m_isReady;
	}

private:
	BackGround*							m_backGround=nullptr;					//ステージのポインタ。
	GameCamera*							m_gameCamera=nullptr;					//カメラのポインタ。
	Goal*								m_goal=nullptr;							//ゴールのポインタ。
	nsK2Engine::SkyCube*				m_skyCube = nullptr;					//スカイキューブ。
	Player*								m_winner = nullptr;						//勝者のプレイヤー。
	CountdownManager*					m_countdown = nullptr;					//カウントダウンのポインタ。
	EnGamePhase							m_gamePhase = enGamePhase_CameraDemo;	//ゲームフェーズ。
	MultiViewRender*					m_multiviewRender;						//マルチビューレンダラー。
	PlayerManager*						m_playerManager;						//プレイヤーマネージャー。
	StageObjectFactory*					m_stageFactory;							//ステージファクトリー。
	std::vector<Player*>				m_players;								//プレイヤーの配列。
	SpriteRender						m_spriteRender;							//スプライトレンダー。
	SpriteRender						m_roundOverSprite;						//ラウンドオーバーの画像。
	FontRender							m_fontRender;							//フォントレンダー。
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
	bool								m_isDemoCamera = true;					//カメラかどうか。
	bool								m_isFadeOut = false;					//フェードアウト中か。
	bool								m_isFadeIn = false;						//フェードイン中か。
	bool								m_isCountdownStart = false;				//カウントダウンが始まったか。
	static bool							m_isReady;								//準備完了か。
public:
	bool								m_isDelete = false;						//削除するか。
};

