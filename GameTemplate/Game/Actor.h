#pragma once


class Actor:public IGameObject
{
public:
	bool Start()override;
	Actor();
	virtual~Actor();
	virtual void Update()=0;//純粋仮想関数。必ず派生先で実装しなければならない。
	void Render(RenderContext& rc)override;

	//座標を取得。
	const Vector3& GetPosition()const
	{
		return m_position;
	}

	//座標を設定。
	void SetPosition(Vector3 position)
	{
		m_position = position;
	}


	void SetRotation(const Quaternion& rot)
	{
		m_rotation = rot;
	}

	Quaternion GetRotation()const
	{
		return m_rotation;
	}

	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}

	const Vector3 GetScale()const
	{
		return m_scale;
	}

protected:

	Vector3 m_position = Vector3::Zero;
	Quaternion m_rotation = Quaternion::Identity;
	Vector3 m_scale = Vector3::One;
	ModelRender m_modelRender;
};

