#include "stdafx.h"
#include "GameCamera.h"
#include "Player.h"
#include "Actor.h"
#include "JsonUtility.h"

namespace
{
	//注視点追従の補間速度。
	const float FOLLOW_SPEED = 0.1f;
	//カメラの基本高さオフセット。
	const float CAMERA_HEIGHT_OFFSET = 150.0f;
	//プレイヤーY方向追従率。
	const float CAMERA_Y_FOLLOW_RATE = 0.5f;
}

bool GameCamera::Start()
{
	//Jsonデータを格納する変数。
	json configData;

	//Jsonファイルを読み込む。
	if (!JsonUtility::LoadJson("Assets/config/GameCamera.json", configData))
	{
		return false;
	}

	//GameCameraノードを取得。
	auto gameCamera = configData["GameCamera"];

	//カメラの相対座標を取得。
	auto pos = gameCamera["Position"];
	m_toCameraPos.Set(pos[0], pos[1], pos[2]);

	//ニア・ファークリップ設定。
	m_nearClip = gameCamera["NearClip"];
	m_farClip = gameCamera["FarClip"];

	//上下回転制限。
	m_cameraMax = gameCamera["CameraMax"];
	m_cameraMin = gameCamera["CameraMin"];

	//注視点オフセット。
	m_target_Y = gameCamera["CameraTargetOffSetY"];
	m_target_Z = gameCamera["CameraTargetOffSetZ"];

	//回転感度。
	m_rotationAngleX = gameCamera["RotationAngleX"];
	m_rotationAngleY = gameCamera["RotationAngleY"];

	//デモカメラ設定を取得。
	auto demo = gameCamera["DemoCamera"];

	auto demoStart = demo["StartPosition"];
	auto demoEnd = demo["EndPosition"];
	auto demoTarget = demo["Target"];

	//デモカメラの座標設定。
	m_demoStartPos.Set(demoStart[0], demoStart[1], demoStart[2]);
	m_demoEndPos.Set(demoEnd[0], demoEnd[1], demoEnd[2]);
	m_demoTarget.Set(demoTarget[0], demoTarget[1], demoTarget[2]);

	//デモカメラの移動時間。
	m_demoDuration = demo["MoveDuration"];

	//カメラのクリップ距離を設定。
	g_camera3D->SetNear(m_nearClip * 2.0f);
	g_camera3D->SetFar(m_farClip * 2.0f);

	//初期カメラ設定。
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
	//無効状態なら更新しない。
	if (!m_enable)
	{
		return;
	}

	//デモカメラ中。
	if (m_cameraMode == enCameraMode_Demo)
	{
		UpdateDemoCamera();
		return;
	}

	//通常追従カメラ。
	CameraMove();
}

void GameCamera::CameraMove()
{
	//プレイヤーの座標を取得。
	Vector3 playerPos = m_player->GetPosition();

	//注視点を計算。
	Vector3 target = playerPos;
	target.y += m_target_Y;
	target.z += m_target_Z;

	//注視点を補間して追従させる。
	static Vector3 lastTarget = target;
	lastTarget += (target - lastTarget) * FOLLOW_SPEED;

	//回転前のカメラ方向を保存。
	Vector3 toCameraPosOld = m_toCameraPos;

	//右スティック入力取得。
	float x = g_pad[0]->GetRStickXF();
	float y = g_pad[0]->GetRStickYF();

	//Y軸回転。
	Quaternion rot;
	rot.SetRotationDeg(Vector3::AxisY, m_rotationAngleY * x);
	rot.Apply(m_toCameraPos);

	//X軸回転。
	Vector3 axisX;
	axisX.Cross(Vector3::AxisY, m_toCameraPos);
	axisX.Normalize();
	rot.SetRotationDeg(axisX, m_rotationAngleX * y);
	rot.Apply(m_toCameraPos);

	//上下回転制限チェック。
	Vector3 dir = m_toCameraPos;
	dir.Normalize();
	if (dir.y < m_cameraMin || dir.y > m_cameraMax)
	{
		m_toCameraPos = toCameraPosOld;
	}

	//プレイヤーの高さ変化を追従。
	static float lastPlayerY = playerPos.y;
	float deltaY = playerPos.y - lastPlayerY;
	lastPlayerY = playerPos.y;

	//カメラ座標を計算。
	Vector3 pos = lastTarget + m_toCameraPos;
	pos.y += CAMERA_HEIGHT_OFFSET + deltaY * CAMERA_Y_FOLLOW_RATE;

	//カメラに反映。
	g_camera3D->SetTarget(lastTarget);
	g_camera3D->SetPosition(pos);
	g_camera3D->Update();
}

void GameCamera::UpdateDemoCamera()
{
	//デモが終了していたら何もしない。
	if (m_demoTimer >= m_demoDuration)
	{
		return;
	}

	//デモ時間更新。
	m_demoTimer += g_gameTime->GetFrameDeltaTime();

	//補間率を計算。
	float t = min(m_demoTimer / m_demoDuration, 1.0f);

	//デモカメラ座標を計算。
	Vector3 pos = m_demoStartPos + (m_demoEndPos - m_demoStartPos) * t;

	//カメラ更新。
	g_camera3D->SetPosition(pos);
	g_camera3D->SetTarget(m_demoTarget);
	g_camera3D->Update();
}

void GameCamera::StartFollow()
{
	//追従カメラへ切り替え。
	m_cameraMode = enCameraMode_Follow;
}
