#pragma once


class Player;
class Goal;

//プレイヤーをゴールへ向かわせるAI制御クラス。
class AIControl : public IGameObject
{
public:
	//コンストラクタ。
	//操作対象となるプレイヤーを受け取る。
	AIControl(Player* player);

	//毎フレーム呼ばれる更新処理。
	void Update();

	//現在の移動方向を取得する。
	const Vector3& GetMoveDir() const
	{
		return m_moveDir;
	}

	//ジャンプ要求が発生しているかを取得する。
	bool IsJumpRequested() const
	{
		return m_jumpRequested;
	}

private:
	//プレイヤーとゴールが有効かどうかを確認する。
	bool IsValid();

	//ゴール方向のベクトルを更新する。
	void UpdateGoalDirection();

	//前方の障害物をチェックする。
	void CheckObstacle();

	//回避行動を開始する。
	void StartAvoid();

	//状態に応じて移動方向を更新する。
	void UpdateMoveDirection();

	//回避中の移動処理を行う。
	void UpdateAvoidMove();

	//ゴール方向への補正を移動方向に加える。
	void ApplyGoalCorrection();

	//ジャンプが必要かどうかを判定する。
	void CheckJump();

private:
	Player*			m_player = nullptr;			//操作対象となるプレイヤーのポインタ。
	Goal*			m_goal = nullptr;			//ゴールオブジェクトのポインタ。
	Vector3			m_toGoal = Vector3::Zero;	//ゴール方向へのベクトル。
	Vector3			m_moveDir = Vector3::Zero;	//現在の移動方向。
	bool			m_isAvoiding = false;		//回避中かどうかを示すフラグ。
	float			m_avoidTimer = 0.0f;		//回避行動の残り時間。
	int				m_avoidDir = 1;				//1なら右方向、-1なら左方向。
	bool			m_jumpRequested = false;	//ジャンプを行うかどうかの要求フラグ。
};
