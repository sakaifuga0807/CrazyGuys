#pragma once


class Actor:public IGameObject
{
public:
	bool Start()override
	{
		return true;
	}

	Actor()=default;//デフォルトコンストラクタをコンパイラに自動生成させる。
	virtual ~Actor() {};
	void Render(RenderContext& rc)override
	{
		m_modelRender.Draw(rc);
	}

	//座標を設定。
	void SetPosition(Vector3 position)
	{
		m_position = position;
	}

	//座標を取得。
	const Vector3& GetPosition()const
	{
		return m_position;
	}

	//前方向を取得。
	const Vector3& GetForward()const
	{
		return m_forward;
	}

	//回転を設定。
	void SetRotation(const Quaternion& rot)
	{
		m_rotation = rot;
	}

	//回転を取得。
	const Quaternion& GetRotation()const
	{
		return m_rotation;
	}

	//大きさを設定。
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}

	//ラジアン角に変換。
	constexpr float ToRadian(float degree)
	{
		return degree * (3.14159265359f / 180.0f);
	}

protected:
	//派生先のクラスで使えるようにする。
	ModelRender			m_modelRender;							//モデルレンダー。
	CollisionObject		*m_collisionObject;						//当たり判定。
	Quaternion			m_rotation = Quaternion::Identity;		//回転。
	Vector3				m_position = Vector3::Zero;				//座標。
	Vector3				m_scale = Vector3::One;					//大きさ。
	Vector3				m_collisionSize = Vector3::Zero;		//コリジョンの大きさ。
	Vector3				m_forward = g_vec3Front;				//前方。
};

