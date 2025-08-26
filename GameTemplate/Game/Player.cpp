#include "stdafx.h"
#include <fstream>
#include "Player.h"
#include "External/nlohmann/json.hpp"
#include <iostream>
using json = nlohmann::json;

bool Player::Start()
{
	//モデルを読み込む。
	m_modelRender.Init("Assets/modelData/unityChan.tkm");

	//Playerのjsonファイルを読み込む。
	std::ifstream file{ "Assets/config/Player.json" };

	//ファイルが開けない場合処理をスキップ。
	if (!file.is_open())
	{
		std::cerr << "jsonファイルが開けませんでした。" << std::endl;
		return false;
	}

	json configData;
	file >> configData;

	//Playerのデータを取得。
	auto playerData = configData["Player"];

	//スティックの移動速度を持ってくる。
	m_stickMoveSpeed = playerData["StickMoveSpeed"];
	//重力を持ってくる。
	m_gravity = playerData["Gravity"];
	//キャラコンのサイズを持ってくる。
	m_characterRadius = playerData["CharacterRadius"];
	m_characterHeight = playerData["CharacterHeight"];
	//座標を持ってくる。
	auto pos = playerData["Position"];
	//ジャンプの移動速度を持ってくる。
	m_jump = playerData["Jump"];

	//座標をセット。
	m_position.Set(pos[0], pos[1], pos[2]);

	//キャラコンを初期化。
	m_characterController.Init(m_characterRadius, m_characterHeight, m_position);

	return true;
}

Player::Player()
{

}

Player::~Player()
{

}

void Player::Update()
{
	m_modelRender.Update();

	//移動処理
	Move();
	//ジャンプ処理。
	Jump();
	//回転処理。
	Rotation();
}

void Player::Move()
{
	//xとzの移動速度を0にする
	m_moveSpeed.x = 0.0f;
	m_moveSpeed.z = 0.0f;

	//左スティックの入力量を取得。
	Vector3 stickL;
	stickL.x = g_pad[0]->GetLStickXF();
	stickL.y = g_pad[0]->GetLStickYF();

	//カメラの前宝庫と右方向のベクトルを持ってくる。
	Vector3 forward = g_camera3D->GetForward();
	Vector3 right = g_camera3D->GetRight();

	//y方向には移動させない。
	forward.y = 0.0f;
	forward.Normalize();
	right.y = 0.0f;
	right.Normalize();
	//XZ成分の移動速度をクリア。
	m_moveSpeed += forward * stickL.y * m_stickMoveSpeed;	//奥方向への移動速度を加算。
	m_moveSpeed += right * stickL.x * m_stickMoveSpeed;		//右方向への移動速度を加算。

	//フレームごとに座標を移動させる。
	m_position = m_characterController.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());

	//キャラコンが地面に当たってしていたら。
	if (m_characterController.IsOnGround())
	{
		//重力をなくす。
		m_moveSpeed.y = 0.0f;
	}
	else
	{
		//重力を発生させる。
		m_moveSpeed.y -= m_gravity;
	}

	//モデルの座標をセットする。
	m_modelRender.SetPosition(m_position);
}

void Player::Jump()
{
	//地面についていればジャンプできる。
	if (g_pad[0]->IsTrigger(enButtonA)&&m_characterController.IsOnGround())
	{
		m_moveSpeed.y += m_jump;
	}
}

void Player::Rotation()
{
	if (fabsf(m_moveSpeed.x) < 0.001f && fabsf(m_moveSpeed.z) < 0.001)
	{
		return;
	}
	//回転角度。
	float angle = atan2(-m_moveSpeed.x, m_moveSpeed.z);

	m_rotation.SetRotationY(-angle);

	//回転を設定する。
	m_modelRender.SetRotation(m_rotation);
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}