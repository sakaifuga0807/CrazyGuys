#pragma once

class BackGround:public IGameObject
{
public:
	//メンバ関数。
	bool Start() ;
	BackGround();
	~BackGround();
	void Update();
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

	//メンバ変数。
private:
	ModelRender m_modelRender;//モデルレンダー。
	PhysicsStaticObject m_physicsStaticObj;//静的物理オブジェクト。
	Vector3 m_position;//座標。
	Vector3 m_scale = Vector3::Zero;//大きさ。
};

