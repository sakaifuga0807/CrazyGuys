#include "stdafx.h"
#include "Goal.h"
#include "Player.h"
#include "collision/CollisionObject.h"

bool Goal::Start()
{
	//モデルの読み込み。
	m_modelRender.Init("Assets/stageData/Key.tkm");

	//インスタンスアドレスを検索。
	m_player = FindGO<Player>("player");

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
	m_rotation.AddRotationDegY(100.0f + g_gameTime->GetFrameDeltaTime());
	//回転をセットする。
	m_modelRender.SetRotation(m_rotation);
}

void Goal::CreateCollision()
{
	//コリジョンオブジェクトの作成。
	m_collisionObj = NewGO<CollisionObject>(0);
	//ボックスコリジョンの作成。
	m_collisionObj->CreateBox(Vector3(-133,95,-38895), Quaternion::Identity, Vector3(300.0f, 400.0f, 300.0f));
	//名前を付ける。
	m_collisionObj->SetName("Goal");
	//自動削除されないようにする。
	m_collisionObj->SetIsEnableAutoDelete(false);
}

void Goal::Render(RenderContext&rc)
{
	m_modelRender.Draw(rc);
}
