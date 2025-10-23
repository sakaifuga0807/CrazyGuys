#include "stdafx.h"
#include "BackGround.h"
#include <cstdio>.

bool BackGround::Start()
{
	//ステージの読み込み。
	m_modelRender.Init("Assets/stageData/StageMini.tkm");

	//静的物理オブジェクトを作成。
	m_physicsStaticObj.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());

	//当たり判定を可視化する。
	//PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();

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