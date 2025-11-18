#include "stdafx.h"
#include "Goal.h"
#include "Player.h"
#include "collision/CollisionObject.h"
#include "JsonUtility.h"

bool Goal::Start()
{
	//モデルの読み込み。
	m_modelRender.Init("Assets/stageData/Key.tkm");

	json configData;
	if (!JsonUtility::LoadJson("Assets/config/Goal.json", configData))
	{
		return false;
	}
	//ノードを取得。
	auto GoalData = configData["Goal"];

	//座標。
	auto pos = GoalData["Position"];
	m_position.Set(pos[0], pos[1], pos[2]);
	m_modelRender.SetPosition(m_position);
	//コリジョンの大きさ。
	auto boxsize = GoalData["BoxCollisionSize"];
	m_boxCollisionSize.Set(boxsize[0], boxsize[1], boxsize[2]);


	//コリジョンの作成。
	CreateCollision();

	return true;
}

Goal::Goal()
{

}

Goal::~Goal()
{
	DeleteGO(m_collisionObj);
}

void Goal::Update()
{
	Rotation();
}

void Goal::Rotation()
{
	//回転を加算する。
	m_rotation.AddRotationDegY(100.0f * g_gameTime->GetFrameDeltaTime());
	//回転をセットする。
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.Update();
}

void Goal::CreateCollision()
{
	//コリジョンオブジェクトの作成。
	m_collisionObj = NewGO<CollisionObject>(0);
	//ボックスコリジョンの作成。
	m_collisionObj->CreateBox(m_position, Quaternion::Identity, m_boxCollisionSize);
	//名前を付ける。
	m_collisionObj->SetName("Goal");
	//自動削除されないようにする。
	m_collisionObj->SetIsEnableAutoDelete(false);

	m_collisionObj->SetUserPointer(this);
}

void Goal::Render(RenderContext&rc)
{
	m_modelRender.Draw(rc);
}
