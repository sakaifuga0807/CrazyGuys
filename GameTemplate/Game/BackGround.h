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

	//メンバ変数。
private:
	ModelRender m_modelRender;//モデルレンダー。
	PhysicsStaticObject m_physicsStaticObject;//静的物理オブジェクト。
};

