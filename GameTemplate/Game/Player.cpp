#include "stdafx.h"
#include "Player.h"
#include "Goal.h"
#include "Game.h"
#include "Hammer.h"
#include "PlayerControl.h"
#include "FadeManager.h"
#include "AIControl.h"
#include "JsonUtility.h"
#include "SoundManager.h"

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
	//m_modelRender.Init("Assets/modelData/BulueGuys.tkm");
	m_modelRender.Init("Assets/modelData/a.tkm", m_animationClips, enAnimationClip_Num);

	//音を読み込む。
	SoundManager::Get().LoadFromJson("Assets/config/Sound.json");

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
	m_fallGravity = playerData["FallGravity"];
	//吹き飛ばす力。
	m_blowPower = playerData["BlowPower"];
	//空気抵抗。
	m_airResistance = playerData["AirResistance"];

	///////////////////////////////////////////////////////////////////////////////
	//終わり。
	///////////////////////////////////////////////////////////////////////////////

	//大きさを設定。
	m_modelRender.SetScale(scale[0], scale[1], scale[2]);

	m_rotation = Quaternion::Identity;
	m_modelRender.SetRotation(m_rotation);

	//座標をセット。
	//m_position.Set(pos[0], pos[1], pos[2]);

	//キャラコンを初期化。
	m_characterController.Init(m_characterRadius, m_characterHeight, m_position);

	//インスタンスアドレスを検索。
	m_goal = FindGO<Goal>("goal");

	if (m_isAI)
	{
		m_aiControl = std::make_unique<AIControl>(this);
	}
	else
	{
		m_playerControl = std::make_unique<PlayerControl>(this);
	}

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
	m_modelRender.SetPosition(m_position);

	//カウントダウン中は動かさない。
	if (!m_canMove)
	{
		return;
	}

	if (m_playerControl)
	{
		m_playerControl->Update();
		m_moveDir = m_playerControl->GetMoveDir();
		m_isJumpRequested = m_playerControl->IsJumpRequested();
	}
	else if (m_aiControl)
	{
		m_aiControl->Update();
		m_moveDir = m_aiControl->GetMoveDir();
		m_isJumpRequested = m_aiControl->IsJumpRequested();
	}

	//移動処理。
	Move(m_moveDir);
	//ジャンプ処理。
	Jump();
	//回転処理。
	Rotation(m_moveDir);
	//一定距離落下したら座標ををリセットする。	
	ResetPosition();
	//コリジョン処理。
	CheckCollision();
	//アニメーション再生。
	PlayAnimation();

	m_modelRender.SetPosition(m_position);
}

//移動処理。
void Player::Move(const Vector3& moveDir)
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

	if (!m_isDiving)
	{
		if (moveDir.LengthSq() > 0.01f)
		{
			m_state = enState_Run;
			m_moveSpeed.x = moveDir.x * m_stickMoveSpeed;
			m_moveSpeed.z = moveDir.z * m_stickMoveSpeed;
		}
		else
		{
			m_state = enState_Idle;
			m_moveSpeed.x = 0.0f;
			m_moveSpeed.z = 0.0f;
		}
	}
	else
	{
		//ダイブ中は入力を受け付けない。
		m_diveTimer -= g_gameTime->GetFrameDeltaTime();

		//徐々に速度を減衰。
		m_moveSpeed.x *= 0.98;
		m_moveSpeed.z *= 0.98;

		//ダイブ時間が終わったらダイブ終了。
		if (m_diveTimer <= 0.0f || m_characterController.IsOnGround())
		{
			m_isDiving = false;
		}
	}

	//重力を発生させる。
	if (!m_characterController.IsOnGround())
	{
		m_moveSpeed.y -= m_gravity;
	}

	//キャラコンでキャラクターを移動させる。
	m_position = m_characterController.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());

	//地面に付いていたら。
	if (m_characterController.IsOnGround())
	{
		m_moveSpeed.y = 0.0f;
		m_isJumping = false;
	}

	m_modelRender.SetPosition(m_position);
	m_modelRender.Update();
}

//ジャンプ処理。
void Player::Jump()
{
	//地面についていればジャンプできる。
	if (m_isJumpRequested && m_characterController.IsOnGround())
	{
		//フラグを戻す。
		m_isJumpRequested = false;
		//ジャンプ中のフラグを立てる。
		m_isJumping = true;
		//ダイブのフラグを立てないようにする。
		m_isDiving = false;


		if (!m_isAI)
		{
			//音を再生。
			SoundManager::Get().Play("Jump");
		}

		//ステートをジャンプにする。
		m_state = enState_Jump;
		//ジャンプ力を加える。
		m_moveSpeed.y = m_jumpPower;

		return;
	}
	//ジャンプ中にもう一度Aボタンを押すとダイブに。
	else if (m_isJumping && !m_isDiving && m_isJumpRequested)
	{
		//フラグを戻す。
		m_isJumpRequested = false;
		//ダイブのフラグを立てる。
		m_isDiving = true;


		//ダイブ継続時間を設定。
		m_diveTimer = m_diveDuration;

		//ステートをダイブにする。
		m_state = enState_Dive;

		Vector3 forward = m_forward;
		forward.y = 0.0f;
		forward.Normalize();

		m_moveSpeed.x = forward.x * m_diveForwardSpeed;
		m_moveSpeed.z = forward.z * m_diveForwardSpeed;

		//上昇中なら上方向の速度をリセット。
		if (m_moveSpeed.y > 0.0f)
		{
			m_moveSpeed.y = 0.0f;
		}
	}
}

//回転処理。
void Player::Rotation(const Vector3& moveDir)
{
	if (moveDir.LengthSq() < 0.001f)
	{
		return;
	}

	m_forward = moveDir;
	float angle = atan2(moveDir.x, moveDir.z);

	//回転をQuaternionに変換。
	Quaternion targetRot;
	targetRot.SetRotationY(angle);
	//スムーズに回転できるように補完する。
	m_rotation.Slerp(0.1f, m_rotation, targetRot);
	//回転を適用。
	m_modelRender.SetRotation(m_rotation);
}

//座標をリセットする。
void Player::ResetPosition()
{
	//AIが落ちた場合はフェードさせない。
	if (m_isAI)
	{
		if (m_position.y <= -3000.0f)
		{
			m_position = m_firstPosition;
			m_characterController.SetPosition(m_firstPosition);
			m_moveSpeed = Vector3::Zero;
			m_isJumping = false;
			m_isDiving = false;
		}
		return;
	}

	auto* Fade = FadeManager::GetInstance();
	//フェード中はキャラを動かさない。
	if (m_isRespawn)
	{
		//フェードが終わっていたらフラグを戻す。
		if (Fade&&!Fade->IsFadeing())
		{
			m_isRespawn = false;
		}
		//フェード中なら抜ける。
		if (m_isRespawn)
		{
			return;
		}
	}

	//落下し始めたら落下音を鳴らす。
	if (m_moveSpeed.y <= -1000.0f)
	{
		if (!m_isFalling)
		{
			if (!m_isAI)
			{
				SoundManager::Get().Play("Fall");
			}
			m_isFalling = true;

		}
	}


	if (m_position.y <= -3000.0f)
	{
		m_isRespawn = true;

		//フェードアウト。
		FadeManager::GetInstance()->StartFadeOut(1.0f, [this]() 
		{
			//最初の位置に戻す。
			m_position = m_firstPosition;
			//キャラコンも最初の位置に戻す。
			m_characterController.SetPosition(m_firstPosition);

			//フラグもリセット。
			m_isJumping = false;
			m_isDiving = false;

			m_modelRender.SetPosition(m_firstPosition);
			m_modelRender.Update();

			if (!m_isAI)
			{
				SoundManager::Get().Play("Respawn");
			}

			//落下音を止める。
			m_isFalling = false;

			//フェードイン。
			FadeManager::GetInstance()->StartFadeIn(1.0f, [this]()
			{
				//フェードイン解除。
				m_isRespawn = false;
			});
		});
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
			if (m_game)
			{
				SoundManager::Get().Play("Win");
				m_game->OnPlayerGoal();
			}

			return;
		}
	}

	//hammerのコリジョン取得。
	const auto& HammerCollisions = g_collisionObjectManager->FindCollisionObjects("Hammer");
	for (auto collision : HammerCollisions)
	{
		if (!collision)
		{
			continue;
		}

		//コリジョンとキャラが当たったら。
		if (collision->IsHit(m_characterController))
		{
			Hammer* hammer = static_cast<Hammer*>(collision->GetUserPointer());
			if (!hammer)
			{
				continue;
			}
			
			//AIだったら音を鳴らさない。
			if (!m_isAI)
			{
				//音を再生。
				SoundManager::Get().Play("HammerHIT");
			}

			//前方向を取得して、正規化。
			Vector3 hammerForward = hammer->GetForward();
			hammerForward.y = 0.0f;
			hammerForward.Normalize();

			//吹き飛ばす力を移動速度に追加。
			m_moveSpeed.x = hammerForward.x * m_blowPower;
			m_moveSpeed.z = hammerForward.z * m_blowPower;

			//ステートをヒットにする。
			m_state = enState_Hit;
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

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
	m_fontRender.Draw(rc);
}