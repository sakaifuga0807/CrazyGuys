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

	///////////////////////////////////////////////////////////////////////////////
	//Jsonファイルの読み込み。
	///////////////////////////////////////////////////////////////////////////////
	
	//ファイルが開けない場合処理をスキップ。
	if (!file.is_open())
	{
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
	//ジャンプ力を持ってくる。
	m_jumpPower = playerData["JumpPower"];
	//スライディング時間を持ってくる。
	m_diveDuration = playerData["DiveDuration"];
	//スライディング時の全身スピードを持ってくる。
	m_diveForwardSpeed = playerData["DiveForwardSpeed"];
	//回転角度を持ってくる
	m_diveRotationAngle = playerData["RotationAngle"];
	//落下時の重力倍率を持ってくる。
	m_fallGravityScale = playerData["FallGravityScale"];

	//ファイルを閉じる。
	file.close();
	///////////////////////////////////////////////////////////////////////////////
	//終わり。
	///////////////////////////////////////////////////////////////////////////////

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
	//移動処理。
	Move();
	//ジャンプ処理。
	Jump();
	//回転処理。
	Rotation();
}

//移動処理。
void Player::Move()
{
	//左スティックの入力量を取得。
	Vector3 stickL;
	stickL.x = g_pad[0]->GetLStickXF();
	stickL.y = g_pad[0]->GetLStickYF();	

	//カメラの前宝庫と右方向のベクトルを持ってくる。
	Vector3 forward = g_camera3D->GetForward();
	Vector3 right = g_camera3D->GetRight();

	//y方向には移動させない。
	forward.y = 0.0f; forward.Normalize();
	right.y = 0.0f; right.Normalize();

	//ダイブでなければ移動処理を行う。
	if (!m_isDiving)
	{
		//xとzの移動速度をなくす。
		m_moveSpeed.x = 0.0f;
		m_moveSpeed.z = 0.0f;

		//XZ成分の移動速度をクリア。
		m_moveSpeed += forward * stickL.y * m_stickMoveSpeed;
		m_moveSpeed += right * stickL.x * m_stickMoveSpeed;
	}
	else
	{
		//スティックが倒されていればその方向に移動。
		if (stickL.Length() > 0.1)
		{
			Vector3 diveDir = forward * stickL.y + right * stickL.x;
			diveDir.Normalize();
			m_moveSpeed.x = diveDir.x * m_diveForwardSpeed;		//奥方向への移動速度を加算。
			m_moveSpeed.z = diveDir.z * m_diveForwardSpeed;		//右方向への移動速度を加算。
		}
		else
		{
			//徐々に減速させる。
			m_moveSpeed.x *= 0.95;
			m_moveSpeed.z *= 0.95;
		}

		//ダイブ中は前傾姿勢にする。
		m_rotation.SetRotationX(ToRadian(m_diveRotationAngle));
		//回転を設定。
		m_modelRender.SetRotation(m_rotation);
	}

	//キャラコンをExecute関数で毎フレーム移動させる
	m_position = m_characterController.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());

	//キャラコンが地面に付いていれば。
	if (m_characterController.IsOnGround())
	{
		//重力をなくす。
		m_moveSpeed.y = 0.0f;
		//フラグを戻す。
		m_isJumping = false;
		m_isDiving = false;

		//回転角度も戻す。
		m_rotation.SetRotationX(0.0f);
		m_modelRender.SetRotation(m_rotation);
	}
	else
	{
		//重力を発生させる。
		m_moveSpeed.y -= m_gravity;
	}

	//モデルの座標をセットする。
	m_modelRender.SetPosition(m_position);
	//モデルを更新する。
	m_modelRender.Update();
}

//ジャンプ処理。
void Player::Jump()
{
	//地面についていればジャンプできる。
	if (g_pad[0]->IsTrigger(enButtonA)&&m_characterController.IsOnGround())
	{
		//ジャンプのフラグを立てる。
		m_isJumping = true;

		m_isDiving = false;
		//ジャンプ力を加える。
		m_moveSpeed.y += m_jumpPower;
	}
	else if (m_isJumping&&!m_isDiving&&g_pad[0]->IsTrigger(enButtonA))
	{
		//ダイブのフラグを立てる。
		m_isDiving = true;
		
		//上昇しているなら慣性を消して一気に落下させる。
		if (m_moveSpeed.y > 0)
		{
			m_moveSpeed.y = 0;
		}

		//ジャンプした瞬間にちょっとだけ前に進める。
		Vector3 forward = g_camera3D->GetForward();
		forward.y = 0.0f; forward.Normalize();


		m_moveSpeed.x = forward.x * m_diveForwardSpeed;
		m_moveSpeed.z = forward.z * m_diveForwardSpeed;
		m_moveSpeed.y -= m_gravity * m_fallGravityScale;	//落下を少しだけ速くする。
	}
}

//回転処理。
void Player::Rotation()
{
	//スティックの入力がなければスキップ。
	if (fabsf(m_moveSpeed.x) < 0.001f && fabsf(m_moveSpeed.z) < 0.001)
	{
		return;
	}

	//回転角度。
	float angle = atan2(-m_moveSpeed.x, m_moveSpeed.z);

	//ラジアンを度に変換。
	m_rotation.SetRotationY(-angle);

	//回転を設定する。
	m_modelRender.SetRotation(m_rotation);
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}