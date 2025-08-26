#include "stdafx.h"
#include "BackGround.h"

bool BackGround::Start()
{
	//当たり判定を有効にする。
	//PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();

	//ステージの読み込み。
	m_modelRender.Init("Assets/modelData/ground.tkm");

	//静的物理オブジェクトを作成。
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());

	return true;
}

BackGround::BackGround()
{

}

BackGround::~BackGround()
{
	
}


void BackGround::Update()
{
	//モデルを更新。
	m_modelRender.Update();
}


void BackGround::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}