#include "stdafx.h"
#include "Player.h"
#include "Goal.h"
#include "Result.h"
#include "Game.h"
#include "Hammer.h"
#include "JsonUtility.h"

bool Player::Start()
{
	//アニメーション読み込み。
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/Idle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Run].Load("Assets/animData/Run.tka");
	m_animationClips[enAnimationClip_Run].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Jump].Load("Assets/animData/Jump.tka");
	m_animationClips[enAnimationClip_Jump].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Dive].Load("Assets/animData/Dive.tka");
	m_animationClips[enAnimationClip_Dive].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Hit].Load("Assets/animData/Hit.tka");
	m_animationClips[enAnimationClip_Hit].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Victory].Load("Assets/animData/Victory.tka");
	m_animationClips[enAnimationClip_Victory].SetLoopFlag(false);

	//モデルを読み込む。
	m_modelRender.Init("Assets/modelData/BulueGuys.tkm");
	//m_modelRender.Init("Assets/modelData/BulueGuys.tkm", m_animationClips, enAnimationClip_Num);

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

	//ノードを取得。
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
	m_modelRender.SetScale(scale[0], scale[1], scale[2]);

	//座標をセット。
	//m_modelRender.SetPosition(m_position);
	m_position.Set(pos[0], pos[1], pos[2]);
	//シーソー前。
	//m_position.Set(8.769943f, 10.0f, -15939.281250f);
	//回転する奴の前
	//m_position.Set(-38.5f, 10.0f, -29253.1f);
	//ゴール前。
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
	//アニメーション再生。
	PlayAnimation();
	//AIかプレイヤーかで処理を分ける。
	if (m_isAI)
	{
		UpdateAIController();
	}
	else
	{
		UpdatePlayerController();
	}

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

		if (m_blowTimer <= 0.0f || m_characterController.IsOnGround())
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
			//ステートを走りに。
			m_state = enState_Run;

			m_moveSpeed.x = (forward.x * lStick_y + right.x * lStick_x) * m_stickMoveSpeed * 2;
			m_moveSpeed.z = (forward.z * lStick_y + right.z * lStick_x) * m_stickMoveSpeed * 2;
		}
		else
		{
			//ステートを待機に。
			m_state = enState_Idle;
			//各値をリセット。
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
		//ジャンプ中のフラグを立てる。
		m_isJumping = true;
		//ダイブのフラグを立てないようにする。
		m_isDiving = false;

		//ステートをジャンプにする。
		m_state = enState_Jump;

		//ジャンプ力を加える。
		m_moveSpeed.y += m_jumpPower;
	}
	//ジャンプ中にもう一度Aボタンを押すとダイブに。
	else if (m_isJumping && !m_isDiving && g_pad[0]->IsTrigger(enButtonA))
	{
		//ダイブのフラグを立てる。
		m_isDiving = true;

		//ステートをダイブにする。
		m_state = enState_Dive;

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

//座標をリセットする。
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

//コリジョンとの当たり判定処理。
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
			//ステートをヒットにする。
			m_state = enState_Hit;

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

void Player::PlayAnimation()
{
	switch (m_state)
	{
		//待機ステートだったら。
	case enState_Idle:
		//待機アニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Idle);
		break;
		//走りステートだったら。
	case enState_Run:
		//走るアニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Run);
		break;
		//ジャンプステートだったら。
	case enState_Jump:
		//ジャンプアニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Jump);
		break;
		//ダイブステートだったら。
	case enState_Dive:
		//ダイブアニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Dive);
		break;
		//ヒットステートだったら。
	case enState_Hit:
		//ヒットアニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Hit);
		break;
		//勝利ステートだったら。
	case enState_Victory:
		//勝利アニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_Victory);
		break;
	default:
		break;
	}
}

void Player::UpdatePlayerController()
{

}

void Player::UpdateAIController()
{

}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
	m_fontRender.Draw(rc);
}
