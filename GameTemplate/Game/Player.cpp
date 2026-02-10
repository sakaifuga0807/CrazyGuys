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

namespace
{
	const float RESET_POSITION = -3000.0f;	//リセットする座標。
	const float PLAY_SOND_RESPAWN = -1000.0;	//落下音を再生する座標。
}

bool Player::Start()
{
	//アニメーション読み込み。
	m_animationClips[enAnimationClip_Idle].Load("Assets/animData/Idle.tka");
	m_animationClips[enAnimationClip_Idle].SetLoopFlag(true);
	m_animationClips[enAnimationClip_Run].Load("Assets/animData/Run.tka");
	m_animationClips[enAnimationClip_Run].SetLoopFlag(true);
	m_animationClips[enAnimationClip_JumpStart].Load("Assets/animData/JumpStart.tka");
	m_animationClips[enAnimationClip_JumpStart].SetLoopFlag(false);
	m_animationClips[enAnimationClip_JumpAir].Load("Assets/animData/JumpAir.tka");
	m_animationClips[enAnimationClip_JumpAir].SetLoopFlag(false);
	m_animationClips[enAnimationClip_JumpEnd].Load("Assets/animData/JumpEnd.tka");
	m_animationClips[enAnimationClip_JumpEnd].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Dive].Load("Assets/animData/Dive.tka");
	m_animationClips[enAnimationClip_Dive].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Hit].Load("Assets/animData/Hit.tka");
	m_animationClips[enAnimationClip_Hit].SetLoopFlag(false);
	m_animationClips[enAnimationClip_Victory].Load("Assets/animData/Victory.tka");
	m_animationClips[enAnimationClip_Victory].SetLoopFlag(false);

	//モデルを読み込む。	
	if (m_modelPath.empty())
	{
		m_modelPath = "Assets/modelData/PinkGuys.tkm";
	}

	m_modelRender.Init(m_modelPath.c_str(), m_animationClips, enAnimationClip_Num);
	m_modelRender.SetShadowCasterFlag(true);

	//エフェクトを読み込む。
	//EffectEmitter

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

	//Gameで設定した座標をそのまま使う。
	m_position = GetPosition();

	//リスポーンも設定。
	m_firstPosition = m_position;

	//キャラコンを初期化。
	m_characterController.Init(m_characterRadius, m_characterHeight, m_position);
	m_modelRender.SetPosition(m_position);
	m_modelRender.Update();

	//正面を向かせる。
	m_forward = Vector3::AxisZ;

	Quaternion initRot;
	initRot.SetRotationDegY(180.0f);
	m_rotation = initRot;
	m_modelRender.SetRotation(m_rotation);

	//インスタンスアドレスを検索。
	m_goal = FindGO<Goal>("goal");

	//エフェクトを読み込む。
	EffectEngine::GetInstance()->ResistEffect(0, u"Assets/effect/Hit.efk");
	EffectEngine::GetInstance()->ResistEffect(1, u"Assets/effect/Respown.efkefc");
	EffectEngine::GetInstance()->ResistEffect(2, u"Assets/effect/jump.efkefc");


	return true;
}

Player::Player()
	:m_controllerIndex(-1)
{

}

Player::~Player()
{

}

void Player::Update()
{
	//カウントダウン中は動かさない。
	if (m_canMove)
	{
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
	}
	else
	{
		//入力を完全停止。
		m_moveDir = Vector3::Zero;
		m_isJumping = false;

		//慣性で動かさないようにする。
		m_moveSpeed.x = 0.0f;
		m_moveSpeed.z = 0.0f;
	}

	//ジャンプ処理。
	Jump();
	//移動処理。
	Move(m_moveDir);
	//入力があるときだけ回転。
	if (m_moveDir.LengthSq() > 0.01f)
	{
		//回転処理。
		Rotation(m_moveDir);
	}
	//一定距離落下したら座標ををリセットする。	
	ResetPosition();
	//コリジョン処理。
	CheckCollision();
	//アニメーション再生。
	PlayAnimation();

	m_modelRender.SetPosition(m_position);
	m_modelRender.Update();
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

	if (m_isDiving)
	{
		m_diveTimer -= g_gameTime->GetFrameDeltaTime();

		m_moveSpeed.y -= m_gravity * m_fallGravity;

		//終了判定。
		if (m_diveTimer <= 0.0f || m_characterController.IsOnGround())
		{
			m_isDiving = false;
		}
	}
	else
	{
		m_moveSpeed.x = moveDir.x * m_stickMoveSpeed;
		m_moveSpeed.z = moveDir.z * m_stickMoveSpeed;

		//重力を発生させる。
		if (!m_characterController.IsOnGround())
		{
			m_moveSpeed.y -= m_gravity;
		}
		else
		{
			if (!m_isJumping &&
				!m_isDiving &&
				!m_isBlown &&
				m_state != enState_Hit &&
				m_state != enState_Victory &&
				m_state != enState_JumpStart &&
				m_state != enState_JumpAir)
			{
				if (moveDir.LengthSq() > 0.001f)
				{
					m_state = enState_Run;
				}
				else
				{
					m_state = enState_Idle;
				}
			}
		}
	}

	//キャラコンでキャラクターを移動させる。
	m_position = m_characterController.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());
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

		EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
		effectEmitter->Init(2);
		effectEmitter->SetPosition(m_position);
		effectEmitter->SetRotation(m_rotation);
		effectEmitter->SetScale({ 75.0f,75.0f,75.0f });
		effectEmitter->Play();

		if (!m_isAI)
		{
			//音を再生。
			SoundManager::Get().Play("Jump");
		}

		//ステートをジャンプにする。
		m_state = enState_JumpStart;
		//ジャンプ力を加える。
		m_moveSpeed.y = m_jumpPower;

		return;
	}

	//ジャンプ中にもう一度Aボタンを押すとダイブに。
	if (m_isJumping && !m_isDiving && m_isJumpRequested)
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

		return;
	}

	if (!m_characterController.IsOnGround() &&m_state == enState_JumpStart)
	{
		if (!m_modelRender.IsPlayingAnimation())
		{
			m_state = enState_JumpAir;
		}
	}

	if (m_characterController.IsOnGround())
	{
		if (m_state == enState_JumpAir)
		{
			m_state = enState_JumpEnd;
			m_isJumping = false;
			m_isDiving = false;
		}
		else if(m_state == enState_Dive)
		{
			m_isDiving = false;
			m_isJumping = false;
		}
	}
}

//回転処理。
void Player::Rotation(const Vector3& moveDir)
{
	Vector3 dir = moveDir;

	if (dir.LengthSq() < 0.001f)
	{
		dir = m_forward;
	}

	dir.Normalize();

	m_forward = dir;
	float angle = atan2(dir.x, dir.z);

	if (!std::isfinite(angle))
	{
		return;
	}

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
		if (m_position.y <= RESET_POSITION)
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
		if (Fade&&!Fade->IsFading())
		{
			m_isRespawn = false;
		}

		//フェード中なら抜ける。
		if (m_isRespawn)
		{
			return;
		}
	}

	//落下し始めたら落下音とエフェクトを再生。
	if (m_moveSpeed.y <= PLAY_SOND_RESPAWN)
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

	//一定距離落下したらリスポン。
	if (m_position.y <= RESET_POSITION)
	{
		m_isRespawn = true;

		//フェードアウト。
		FadeManager::GetInstance()->StartFadeOut(1.0f, [this]() 
		{
			Respawn();

			//フェードイン。
			FadeManager::GetInstance()->StartFadeIn(1.0f, [this]()
			{
				//フェードイン解除。
				m_isRespawn = false;
			});

			EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
			effectEmitter->Init(1);
			effectEmitter->SetPosition(m_position);
			effectEmitter->SetRotation(m_rotation);
			effectEmitter->SetScale({ 200.0f,200.0f,200.0f });
			effectEmitter->Play();
		});
	}
}

void Player::Respawn()
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

	//AIの音は出さない。
	if (!m_isAI)
	{
		SoundManager::Get().Play("Respawn");
	}

	//落下音を止める。
	m_isFalling = false;
}

void Player::SetControllerIndex(int index)
{
	m_controllerIndex = index;

	if (m_controllerIndex >= 0)
	{
		m_isAI = false;
		m_playerControl = std::make_unique<PlayerControl>(this, m_controllerIndex);
		m_aiControl.reset();
	}
	else
	{
		m_isAI = true;
		m_aiControl = std::make_unique<AIControl>(this);
		m_playerControl.reset();
	}
}

//コリジョンとの当たり判定処理。
void Player::CheckCollision()
{
	if (!m_isGoalReached)
	{
		//コリジョンの取得。
		const auto& collisions = g_collisionObjectManager->FindCollisionObjects("Goal");
		//コリジョンの配列をfor文で回す。
		for (auto collision : collisions)
		{
			//コリジョンとキャラが当たったら。
			if (collision->IsHit(m_characterController))
			{
				m_isGoalReached = true;
				if (m_game)
				{
					SoundManager::Get().Play("Win");
					m_game->OnPlayerGoal(this);
				}

				return;
			}
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

			//エフェクトを再生。
			EffectEmitter* effectEmitter = NewGO<EffectEmitter>(0);
			effectEmitter->Init(0);
			effectEmitter->SetPosition(m_position);
			effectEmitter->SetRotation(m_rotation);
			effectEmitter->SetScale({ 20.0f,20.0f,20.0f });
			effectEmitter->Play();

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
	case enState_JumpStart:
		//ジャンプアニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_JumpStart);
		break;
		//空中にいるステートだったら。
	case enState_JumpAir:
		//空中にいるアニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_JumpAir);
		break;
		//着地ステートだったら。
	case enState_JumpEnd:
		//着地アニメーションを再生。
		m_modelRender.PlayAnimation(enAnimationClip_JumpEnd);

		//着地アニメーションが終わったらステートを変更。
		if (!m_modelRender.IsPlayingAnimation())
		{
			if (m_moveDir.LengthSq() > 0.001f)
			{
				m_state = enState_Run;
			}
			else
			{
				m_state = enState_Idle;
			}
		}

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
		//再生した後におかしくならないようにする。
		if (!m_modelRender.IsPlayingAnimation())
		{
			if (m_characterController.IsOnGround())
			{
				if (m_moveDir.LengthSq() > 0.001f)
				{
					m_state = enState_Run;
				}
				else
				{
					m_state = enState_Idle;
				}
			}
		}
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