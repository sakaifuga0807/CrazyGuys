#include "stdafx.h"
#include "GameCamera.h"
#include "Player.h"
#include "Actor.h"
#include "JsonUtility.h"

bool GameCamera::Start()
{
	///////////////////////////////////////////////////////////////////////////////
	//Jsonファイルの読み込み。
	///////////////////////////////////////////////////////////////////////////////

	//Jsonデータを格納する変数。
	json configData;
	//Jsonファイルを読み込む。
	if (!JsonUtility::LoadJson("Assets//config/GameCamera.json", configData))
	{
		return false;
	}

	//ノードを取得。
	auto gameCamera = configData["GameCamera"];

	//座標を持ってくる。
	auto pos = gameCamera["Position"];
	//ニアクリップを持ってくる。
	m_nearClip = gameCamera["NearClip"];
	//ファークリップを持ってくる。
	m_farClip = gameCamera["FarClip"];
	//カメラの上下を制限。
	m_cameraMax = gameCamera["CameraMax"];
	m_cameraMin = gameCamera["CameraMin"];
	//注視点を持ってくる。
	m_target_Y = gameCamera["CameraTargetOffSetY"];
	m_target_Z = gameCamera["CameraTargetOffSetZ"];
	//XとYの回転角度を持ってくる。
	m_rotationAngleX = gameCamera["RotationAngleX"];
	m_rotationAngleY = gameCamera["RotationAngleY"];

	///////////////////////////////////////////////////////////////////////////////
	//終わり。
	///////////////////////////////////////////////////////////////////////////////

	//注視点から視点までのベクトルを設定。
	m_toCameraPos.Set(pos[0], pos[1], pos[2]);

	//プレイヤーのインスタンスを探す。
	m_player = FindGO<Player>("player");

	//カメラのニアクリップとファークリップを設定。
	g_camera3D->SetNear(m_nearClip);
	g_camera3D->SetFar(m_farClip);

	return true;
}

GameCamera::GameCamera()
{

}

GameCamera::~GameCamera()
{

}

void GameCamera::Update()
{
	CameraMove();
}

void GameCamera::CameraMove()
{
	//プレイヤーの座標を取得。
	Vector3 playerPos = m_player->GetPosition();

	//注視点を計算する。
	Vector3 target = playerPos;
	target.y += m_target_Y;
	target.z += m_target_Z;

	//前フレームの注視点を保持して補完。
	static Vector3 lastTarget = target;
	float followSpeed = 0.1f;
	lastTarget = lastTarget + (target - lastTarget) * followSpeed;

	//プレイヤーの注視点を設定。
	Vector3 toCameraPosOld = m_toCameraPos;

	//右スティックを使ってカメラを回す。
	float x = g_pad[0]->GetRStickXF();
	float y = g_pad[0]->GetRStickYF();

	//Y軸周りの回転。
	Quaternion qRot;
	qRot.SetRotationDeg(Vector3::AxisY, m_rotationAngleY * x);
	qRot.Apply(m_toCameraPos);

	//X軸周りの回転。
	Vector3 axisX;
	axisX.Cross(Vector3::AxisY, m_toCameraPos);
	axisX.Normalize();
	qRot.SetRotationDeg(axisX, m_rotationAngleX * y);
	qRot.Apply(m_toCameraPos);

	//カメラの回転の上限をチェックする。
	Vector3 toPosDir = m_toCameraPos;
	toPosDir.Normalize();
	if (toPosDir.y<m_cameraMin || toPosDir.y>m_cameraMax)
	{
		m_toCameraPos = toCameraPosOld;
	}

	//プレイヤーの高さで追従させる。
	static float lastPlayerY = playerPos.y;
	float deltaY = playerPos.y - lastPlayerY;
	lastPlayerY = playerPos.y;

	//カメラの座標を計算。
	Vector3 pos = lastTarget + m_toCameraPos;
	pos.y += 150.0f + deltaY * 0.5f;

	//メインカメラに注視点と座標を設定。
	g_camera3D->SetTarget(lastTarget);
	g_camera3D->SetPosition(pos);
	g_camera3D->Update();
}
