#include "stdafx.h"
#include "Hammer.h"
#include "JsonUtility.h"

bool Hammer::Start()
{
	//モデルの読み込み。
	m_modelRender.Init("Assets/objectData/Hammer.tkm");

	///////////////////////////////////////////////////////////////////////////////
	//Jsonファイルの読み込み。
	///////////////////////////////////////////////////////////////////////////////

	//Jsonデータを格納する変数。
	json configData;

	//Jsonデータを読み込む。
	if (!JsonUtility::LoadJson("Assets/config/hammer.json", configData))
	{
		return false;
	}

	//ノードを取得。
	auto hammerData = configData["Hammer"];

	//コリジョンの大きさを設定。
	auto size = hammerData["CollisionSize"];
	//回転速度を取得。
	m_rotationSpeed = hammerData["RotationSpeed"];

	///////////////////////////////////////////////////////////////////////////////
	//終わり。
	///////////////////////////////////////////////////////////////////////////////

	//代入。
	m_modelRender.SetPosition(m_position);
	m_collisionSize.Set(size[0], size[1], size[2]);

	//コリジョンを作成。
	CreateCollision();

	return true;
}

Hammer::Hammer()
{

}

Hammer::~Hammer()
{
	DeleteGO(m_collisionObject);
}

void Hammer::Update()
{
	//回転させる。
	Rotation();

	//モデルの更新。
	m_modelRender.Update();
}

void Hammer::Rotation()
{
	m_rotation.AddRotationY(m_rotationSpeed);
	m_modelRender.SetRotation(m_rotation);
	m_modelRender.SetPosition(m_position);

	//コリジョンの回転を更新。
	if (m_collisionObject)
	{
		m_collisionObject->SetPosition(m_position);
		m_collisionObject->SetRotation(m_rotation);
	}
}

void Hammer::CreateCollision()
{
	//コリジョンを作成。
	m_collisionObject = NewGO<CollisionObject>(0, "HammerCollision");
	m_collisionObject->CreateBox(m_position, m_rotation, m_collisionSize);
	m_collisionObject->SetIsEnableAutoDelete(false);
	m_collisionObject->SetName("Hammer");

	//ハンマー自身を紐づける。
	m_collisionObject->SetUserPointer(this);
}

void Hammer::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}