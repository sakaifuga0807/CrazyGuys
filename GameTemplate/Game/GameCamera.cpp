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
	//注視点から視点までのベクトルを設定。
	m_toCameraPos.Set(pos[0], pos[1], pos[2]);
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

	//ノードを取得。
	auto demo = gameCamera["DemoCamera"];

	//デモカメラのパラメータを持ってくる。
	auto demoStart = demo["StartPosition"];
	auto demoEnd = demo["EndPosition"];
	auto demoTarget = demo["Target"];

	m_demoStartPos.Set(demoStart[0], demoStart[1], demoStart[2]);
	m_demoEndPos.Set(demoEnd[0], demoEnd[1], demoEnd[2]);
	m_demoTarget.Set(demoTarget[0], demoTarget[1], demoTarget[2]);

	m_demoDuration = demo["MoveDuration"];

	///////////////////////////////////////////////////////////////////////////////
	//終わり。
	///////////////////////////////////////////////////////////////////////////////

	//カメラのニアクリップとファークリップを設定。
	g_camera3D->SetNear(m_nearClip * 2);
	g_camera3D->SetFar(m_farClip * 2);

	g_camera3D->SetPosition(m_demoStartPos);
	g_camera3D->SetTarget(m_demoTarget);

	//初期化。
	m_demoTimer = 0.0f;
	m_cameraMode = enCameraMode_Demo;

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
	if (m_enable == false)
	{
		return;
	}

	if (m_cameraMode == enCameraMode_Demo)
	{
		UpdateDemoCamera();

		return;
	}

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

void GameCamera::UpdateDemoCamera()
{
	if (m_demoTimer >= m_demoDuration)
	{
		return;
	}

	m_demoTimer += g_gameTime->GetFrameDeltaTime();

	float t = min(m_demoTimer / m_demoDuration, 1.0f);

	Vector3 pos = m_demoStartPos + (m_demoEndPos - m_demoStartPos) * t;

	g_camera3D->SetPosition(pos);
	g_camera3D->SetTarget(m_demoTarget);
	g_camera3D->Update();

}

void GameCamera::StartFollow()
{
	m_cameraMode = enCameraMode_Follow;
}