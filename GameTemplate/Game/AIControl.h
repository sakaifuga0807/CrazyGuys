#pragma once

class Player;
class Goal;

class AIControl:public IGameObject
{
public:
	AIControl(Player* player);
	void Update();

	//移動方向を取得。
	const Vector3& GetMoveDir()const
	{
		return m_moveDir;
	}

	//ジャンプを要求。
	bool IsJumpRequested()const
	{
		return m_jumpRequested;
	}

private:
	Player*		m_player = nullptr;					//プレイヤーのポインタ。
	Goal*		m_goal = nullptr;					//ゴールのポインタ。
	Vector3		m_moveDir = Vector3::Zero;			//移動方向。
	int			m_avoidDir = 0.0f;					//左右回避。
	float		m_avoidDistance = 0.0f;				//回転時のy方向の距離。
	float		m_avoidTimer = 0.0f;				//回避中の残り時間。
	bool		m_jumpRequested = false;			//ジャンプ要求。
	bool		m_isAvoiding = false;				//回避できるか。
};

