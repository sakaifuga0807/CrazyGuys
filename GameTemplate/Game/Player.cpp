#include "stdafx.h"
#include "Player.h"
#include "Goal.h"
#include "Result.h"
#include "Game.h"
#include "Hammer.h"
#include "JsonUtility.h"

bool Player::Start()
{
	//モデルを読み込む。
	//m_modelRender.Init("Assets/modelData/unityChan.tkm");
	m_modelRender.Init("Assets/modelData/BulueGuys.tkm");

	//AnimationClipの読み込み。
	/*static AnimationClip clips[4];
	clips[0].Load("Assets/animData/Idle.tka");
	clips[1].Load("Assets/animData/Run.tka");
	clips[2].Load("Assets/animData/Jump.tka");
	clips[3].Load("Assets/animData/Dive.tka");*/

	///////////////////////////////////////////////////////////////////////////////
	//Jsonファイルの読み込み。
	//////////////////////////////////////////////////////////////////////////////
	
	//Jsonデータを格納する変数。
	json configData;
	//Jsonファイルを読み込む。
	if (!JsonUtility::LoadJson("Assets/config/Player.json", configData))
	{
		return false;//読み込み失敗したらスキップ。
	}

	//プレイヤーの値をメンバ変数に格納。
	auto playerData = configData["Player"];

	//座標。
	auto pos = playerData["Position"];
	//大きさ。
	auto scale = playerData["Scale"];
	//スティックの移動速度。
	m_stickMoveSpeed = playerData["StickMoveSpeed"];
	//重力。
	m_gravity = playerData["Gravity"];
	//キャラコンのサイズ。
	m_characterRadius = playerData["CharacterRadius"];
	m_characterHeight = playerData["CharacterHeight"];
	//ジャンプ力。
	m_jumpPower = playerData["JumpPower"];
	//スライディング時間。
	m_diveDuration = playerData["DiveDuration"];
	//スライディング時の全身スピード。
	m_diveForwardSpeed = playerData["DiveForwardSpeed"];
	//回転角度。
	m_diveRotationAngle = playerData["RotationAngle"];
	//落下時の重力倍率。
	m_fallGravityScale = playerData["FallGravity"];
	//吹き飛ばす力。
	m_blowPower = playerData["BlowPower"];
	//空気抵抗。
	m_airResistance = playerData["AirResistance"];

	///////////////////////////////////////////////////////////////////////////////
	//終わり。
	///////////////////////////////////////////////////////////////////////////////

	//大きさを設定。
	m_modelRender.SetScale(scale[0],scale[1],scale[2]);

	//座標をセット。
	m_position.Set(pos[0], pos[1], pos[2]);
	//m_position.Set(-82.0f, 108.0f, -38145.0f);

	//キャラコンを初期化。
	m_characterController.Init(m_characterRadius, m_characterHeight, m_position);

	//インスタンスアドレスを検索。
	m_goal = FindGO<Goal>("goal");
	m_game = FindGO<Game>("game");
	m_hammer = FindGO<Hammer>("hammer");

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
	//コリジョン処理。
	CheckCollision();



	//今だけ座標を表示。(使わなくなったらけしてねー)
	wchar_t wcsbuf[256];
	swprintf_s(wcsbuf, 256, L"Pos: (%f, %f, %f)", m_position.x, m_position.y, m_position.z);
	m_fontRender.SetText(wcsbuf);
	m_fontRender.SetPosition(Vector3(0.0f, 430.0f, 0.0f));
}

//移動処理。
void Player::Move()
{
	if (m_isBlown)
	{
		m_position = m_characterController.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());
		m_blowTimer -= g_gameTime->GetFrameDeltaTime();

		//空気抵抗を加える。
		m_moveSpeed *= m_airResistance;
		//重力を加える。
		m_moveSpeed.y -= m_gravity;

		if (m_blowTimer <= 0.0f||m_characterController.IsOnGround())
		{
			m_isBlown = false;
			m_moveSpeed = Vector3::Zero;
		}

		m_modelRender.SetPosition(m_position);
		m_modelRender.Update();

		return;

	}

	//ダイブ中ならスティック移動は無効化。
	if (m_isDiving) 
	{
		//ダイブの慣性を少しだけ減衰させる。
		m_moveSpeed.x *= 0.9f;
		m_moveSpeed.z *= 0.9f;
	}
	else 
	{
		//スティックの入力量を取得。
		float lStick_x = g_pad[0]->GetLStickXF();
		float lStick_y = g_pad[0]->GetLStickYF();

		Vector3 forward = g_camera3D->GetForward();
		Vector3 right = g_camera3D->GetRight();

		forward.y = 0.0f; forward.Normalize();
		right.y = 0.0f; right.Normalize();

		if (fabsf(lStick_x) > 0.001f || fabsf(lStick_y) > 0.001f) 
		{
			m_moveSpeed.x = (forward.x * lStick_y + right.x * lStick_x) * m_stickMoveSpeed * 2;
			m_moveSpeed.z = (forward.z * lStick_y + right.z * lStick_x) * m_stickMoveSpeed * 2;
		}
		else 
		{
			m_moveSpeed.x = 0.0f;
			m_moveSpeed.z = 0.0f;
		}
	}

	m_position = m_characterController.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());

	//地面に付いたら各種リセット。
	if (m_characterController.IsOnGround()) 
	{
		m_moveSpeed.y = 0.0f;
		m_isDiving = false;  
		m_isJumping = false;
	}
	else
	{
		m_moveSpeed.y -= m_gravity;
	}

	m_modelRender.SetPosition(m_position);
	m_modelRender.Update();
}

//ジャンプ処理。
void Player::Jump()
{
	//地面についていればジャンプできる。
	if (g_pad[0]->IsTrigger(enButtonA) && m_characterController.IsOnGround())
	{
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
			m_moveSpeed.y = 0;
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

void Player::CheckCollision()
{
	//コリジョンの取得。
	const auto& collisions = g_collisionObjectManager->FindCollisionObjects("Goal");
	//コリジョンの配列をfor文で回す。
	for (auto collision : collisions)
	{
		//コリジョンとキャラが当たったら。
		if (collision->IsHit(m_characterController))
		{
			NewGO<Result>(0, "result");
			m_game->m_isDelete = true;
		}
	}

	//hammerのコリジョン取得。
	const auto& HammerCollisions = g_collisionObjectManager->FindCollisionObjects("Hammer");
	for (auto collision : HammerCollisions)
	{
		//コリジョンとキャラが当たったら。
		if (collision->IsHit(m_characterController))
		{
			//前方向を取得して、正規化。
			Vector3 hammerForward = m_hammer->GetForward();
			hammerForward.y = 0.0f;
			hammerForward.Normalize();

			//吹き飛ばす力を移動速度に追加。
			m_moveSpeed.x = hammerForward.x * m_blowPower;
			m_moveSpeed.z = hammerForward.z * m_blowPower;

			m_isBlown = true;
			m_blowTimer = 0.5f;

			return;
		}
	}
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
	m_fontRender.Draw(rc);
}