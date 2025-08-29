#include "stdafx.h"
#include <fstream>
#include <iostream>
#include "External/nlohmann/json.hpp"
#include "GameCamera.h"
#include "Player.h"

using json = nlohmann::json;

bool GameCamera::Start()
{
	//GameCameraのJsonファイルを読み込む。
	std::ifstream file{"Assets/config/GameCamera.json"};

	//ファイルが開けなければスキップ。
	if (!file.is_open()) 
	{
		MessageBox(NULL,L"GameCameraのJsonファイルが開けません。", L"エラー", MB_OK);
		return false;
	}

	json cameraConfig;
	file >> cameraConfig;

	//Jsonの値をメンバ変数に格納。
	auto gameCamera = cameraConfig["GameCamera"];

	//座標を持ってくる。
	auto pos = gameCamera["Position"];
	//ニアクリップを持ってくる。
	m_nearClip = gameCamera["NearClip"];
	//ファークリップを持ってくる。
	m_farClip = gameCamera["FarClip"];
	//注視点を持ってくる。
	m_target_Y = gameCamera["CameraTargetOffSetY"];
	m_target_Z = gameCamera["CameraTargetOffSetZ"];

	//注視点から視点までのベクトルを設定。
	m_toCameraPos.Set(pos[0], pos[1], pos[2]);

	//ファイルを閉じる。
	file.close();

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
	//注視点を計算する。
	Vector3 target = m_player->GetPosition();
	target.y += m_target_Y;
	target.z += m_target_Z;

	//プレイヤーの注視点を設定。
	Vector3 toCameraPosOld = m_toCameraPos;

	//右スティックを使ってカメラを回す。
	float x = g_pad[0]->GetRStickXF();
	float y = g_pad[0]->GetRStickYF();

	//Y軸周りの回転。
	Quaternion qRot;
	qRot.SetRotationDeg(Vector3::AxisY, 1.8f * x);
	qRot.Apply(m_toCameraPos);

	//X軸周りの回転。
	Vector3 axisX;
	axisX.Cross(Vector3::AxisY, m_toCameraPos);
	axisX.Normalize();
	qRot.SetRotationDeg(axisX, 2.1f * y);
	qRot.Apply(m_toCameraPos);

	//カメラの回転の上限をチェックする。
	Vector3 toPosDir = m_toCameraPos;
	toPosDir.Normalize();
	if (toPosDir.y > m_cameraYMAX)
	{
		//カメラが上向き過ぎ。
		m_toCameraPos = toCameraPosOld;
	}
	if (toPosDir.y < m_cameraYMIN)
	{
		//カメラが下向き過ぎ。
		m_toCameraPos = toCameraPosOld;
	}

	//視点を計算する。
	Vector3 pos = target + m_toCameraPos;

	//メインカメラに注視点と座標を設定。
	g_camera3D->SetTarget(target);
	g_camera3D->SetPosition(pos);

	//カメラの更新。
	g_camera3D->Update();
}
