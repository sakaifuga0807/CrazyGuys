#pragma once

class Player;

class Goal:public IGameObject
{
public:
	bool Start();
	Goal();
	~Goal();
	void Update();
	//回転の設定。
	void Rotation();
	//コリジョンの作成。
	void CreateCollision();
	void Render(RenderContext& rc);

	//座標を設定。
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}

	//座標を取得。
	const Vector3& GetPosition() const
	{
		return m_position;
	}

	//大きさを取得。
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}

private:
	//メンバ変数。
	ModelRender			m_modelRender;						//モデルレンダー。
	Vector3				m_position=Vector3::Zero;			//座標。
	Quaternion			m_rotation;							//回転。
	Vector3				m_scale;							//大きさ。
	CollisionObject*	m_collisionObj;						//コリジョンオブジェクト。
	Vector3				m_boxCollisionSize=Vector3::Zero;	//コリジョンの大きさ。
};

