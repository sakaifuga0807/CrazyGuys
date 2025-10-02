#pragma once


class Actor:public IGameObject
{
public:
	bool Start()override
	{
		return true;
	}
	Actor()=default;//デフォルトコンストラクタをコンパイラに自動生成させる。
	virtual~Actor() {};
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

	//回転を設定。
	void SetRotation(const Quaternion& rot)
	{
		m_rotation = rot;
	}

	const Quaternion& GetRotation()const
	{
		return m_rotation;
	}

	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}

protected:
	//派生先のクラスで使えるようにする。
	Quaternion m_rotation = Quaternion::Identity;//回転。
	ModelRender m_modelRender;//モデルレンダー。
	Vector3 m_position = Vector3::Zero;//座標。
	Vector3 m_scale = Vector3::One;//大きさ。
};

