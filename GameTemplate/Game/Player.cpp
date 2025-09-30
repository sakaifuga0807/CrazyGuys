#include "stdafx.h"
#include "External/nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include "Player.h"


using json = nlohmann::json;

bool Player::Start()
{
	//モデルを読み込む。
	//m_modelRender.Init("Assets/modelData/unityChan.tkm");
	m_modelRender.Init("Assets/modelData/BulueGuys.tkm");

	///////////////////////////////////////////////////////////////////////////////
	//Jsonファイルの読み込み。
	///////////////////////////////////////////////////////////////////////////////

	//Playerのjsonファイルを読み込む。
	std::ifstream file{ "Assets/config/Player.json" };

	//ファイルが開けない場合処理をスキップ。
	if (!file.is_open())
	{
		MessageBox(NULL, L"PlayerのJsonファイルが開けません。", L"エラー", MB_OK);
		return false;
	}

	json configData;
	file >> configData;

	//プレイヤーの値をメンバ変数に格納。
	auto playerData = configData["Player"];

	//座標を持ってくる。
	auto pos = playerData["Position"];
	//スティックの移動速度を持ってくる。
	m_stickMoveSpeed = playerData["StickMoveSpeed"];
	//重力を持ってくる。
	m_gravity = playerData["Gravity"];
	//キャラコンのサイズを持ってくる。
	m_characterRadius = playerData["CharacterRadius"];
	m_characterHeight = playerData["CharacterHeight"];
	//ジャンプ力を持ってくる。
	m_jumpPower = playerData["JumpPower"];
	//スライディング時間を持ってくる。
	m_diveDuration = playerData["DiveDuration"];
	//スライディング時の全身スピードを持ってくる。
	m_diveForwardSpeed = playerData["DiveForwardSpeed"];
	//回転角度を持ってくる
	m_diveRotationAngle = playerData["RotationAngle"];
	//落下時の重力倍率を持ってくる。
	m_fallGravityScale = playerData["FallGravity"];

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
	//一定距離落下したら座標ををリセットする。	
	ResetPosition();

	//今だけ座標を表示。(使わなくなったらけしてねー)
	wchar_t wcsbuf[256];
	swprintf_s(wcsbuf, 256, L"Pos: (%f, %f, %f)", m_position.x, m_position.y, m_position.z);

	m_fontRender.SetText(wcsbuf);
	m_fontRender.SetPosition(Vector3(0.0f, 430.0f, 0.0f));
}

//移動処理。
void Player::Move()
{

	// ダイブ中ならスティック移動は無効化
	if (m_isDiving) {
		// ダイブの慣性をちょっと減速させても良い
		m_moveSpeed.x *= 0.9f;
		m_moveSpeed.z *= 0.9f;
	}
	else {
		// スティックの入力量を取得
		float lStick_x = g_pad[0]->GetLStickXF();
		float lStick_y = g_pad[0]->GetLStickYF();

		Vector3 forward = g_camera3D->GetForward();
		Vector3 right = g_camera3D->GetRight();

		forward.y = 0.0f; forward.Normalize();
		right.y = 0.0f; right.Normalize();

		if (fabsf(lStick_x) > 0.001f || fabsf(lStick_y) > 0.001f) {
			m_moveSpeed.x = (forward.x * lStick_y + right.x * lStick_x) * m_stickMoveSpeed * 2;
			m_moveSpeed.z = (forward.z * lStick_y + right.z * lStick_x) * m_stickMoveSpeed * 2;
		}
		else {
			m_moveSpeed.x = 0.0f;
			m_moveSpeed.z = 0.0f;
		}
	}

	// キャラコンを実行
	m_position = m_characterController.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());

	if (m_characterController.IsOnGround()) {
		m_moveSpeed.y = 0.0f;
		m_isDiving = false;   // 地面についたらダイブ終了
		m_isJumping = false;
	}
	else {
		m_moveSpeed.y -= m_gravity;
	}

	m_modelRender.SetPosition(m_position);
	m_modelRender.Update();


	/*//m_moveSpeed.x = 0.0f;
	//m_moveSpeed.z = 0.0f;

	//左スティックの入力量を取得。
	float lStick_x = g_pad[0]->GetLStickXF();
	float lStick_y = g_pad[0]->GetLStickYF();

	//カメラの前宝庫と右方向のベクトルを持ってくる。
	Vector3 forward = g_camera3D->GetForward();
	Vector3 right = g_camera3D->GetRight();

	//XZ平面での前方向、右方向に変換する。
	forward.y = 0.0f; forward.Normalize();
	right.y = 0.0f; right.Normalize();
	//XZ成分の移動速度をクリア。
	m_moveSpeed += forward * lStick_x * m_stickMoveSpeed;
	m_moveSpeed += right * lStick_y * m_stickMoveSpeed;

	if (fabsf(lStick_x) > 0.001 || fabsf(lStick_y) > 0.001)
	{
		m_moveSpeed.x = (forward.x * lStick_y + right.x * lStick_x) * m_stickMoveSpeed * 2;
		m_moveSpeed.z = (forward.z * lStick_y + right.z * lStick_x) * m_stickMoveSpeed * 2;
	}
	else
	{
		//m_moveSpeed.x = 0.95f;
		//m_moveSpeed.z = 0.95f;
	}

	//キャラコンをExecute関数で毎フレーム移動させる
	m_position = m_characterController.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());

	//キャラコンが地面に付いていれば。
	if (m_characterController.IsOnGround())
	{
		//重力をなくす。
		m_moveSpeed.y = 0.0f;
		//フラグを戻す。
		m_isDiving = false;
		m_isJumping = false;
	}
	else
	{
		//重力を発生させる。
		m_moveSpeed.y -= m_gravity;
	}

	//モデルの座標をセットする。
	m_modelRender.SetPosition(m_position);
	//モデルを更新する。
	m_modelRender.Update();*/
}

//ジャンプ処理。
void Player::Jump()
{
	//地面についていればジャンプできる。
	if (g_pad[0]->IsTrigger(enButtonA) && m_characterController.IsOnGround())
	{
		//ジャンプのフラグを立てる。
		m_isJumping = true;

		m_isDiving = false;
		//ジャンプ力を加える。
		m_moveSpeed.y += m_jumpPower;
	}
	else if (m_isJumping && !m_isDiving && g_pad[0]->IsTrigger(enButtonA))
	{
		//ダイブのフラグを立てる。
		m_isDiving = true;

		//上昇しているなら慣性を消して一気に落下させる。
		if (m_moveSpeed.y > 0)
		{
			//m_moveSpeed.y = 0;
		}

		Vector3 forward = m_forward;
		forward.y = 0.0f;
		forward.Normalize();

		m_moveSpeed.x = forward.x * m_diveForwardSpeed;
		m_moveSpeed.z = forward.z * m_diveForwardSpeed;
		m_moveSpeed.y = -m_gravity * m_fallGravityScale * m_fallGravityScale;
	}
}

//回転処理。
void Player::Rotation()
{
	float stickX = g_pad[0]->GetLStickXF();
	float stickY = g_pad[0]->GetLStickYF();

	//スティックの入力がなければスキップ。
	if (fabsf(stickX) < 0.001 && fabsf(stickY) < 0.001)
	{
		return;
	}

	//カメラを基準に方向を決める
	Vector3 forward = g_camera3D->GetForward();
	Vector3 right = g_camera3D->GetRight();
	forward.y = 0.0f;
	forward.Normalize();
	right.y = 0.0f; 
	right.Normalize();

	//入力方向。
	Vector3 dir = forward * stickY + right * stickX;
	dir.Normalize();

	m_forward = dir;

	float angle = atan2(dir.x, dir.z);

	//回転をQuaternionに変換。
	Quaternion targetRot;
	targetRot.SetRotationY(angle);

	//スムーズに補完。
	m_rotation.Slerp(0.1f, m_rotation, targetRot);

	//回転を設定する。
	m_modelRender.SetRotation(m_rotation);
}

void Player::ResetPosition()
{
	if (m_position.y <= -3000.0f)
	{
		//最初の位置に戻す。
		m_position = m_firstPosition;
		//キャラコンも最初の位置に戻す。
		m_characterController.SetPosition(m_position);

		//状態フラグもリセットしておく。
		m_isJumping = false;
		m_isDiving = false;

	}
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
	m_fontRender.Draw(rc);
}