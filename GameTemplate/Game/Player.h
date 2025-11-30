#pragma once

#include "ICharacter.h"

class Goal;
class Game;
class Actor;
class Hammer;
class PlayerControl;
class AIControl;

class Player:public ICharacter
{
public:

	//ステートの列挙型。
	enum EnState
	{
		enState_Idle,
		enState_Run,
		enState_JumpStart,
		enState_JumpAir,
		enState_JumpEnd,
		enState_Dive,
		enState_Hit,
		enState_Victory
	};

	//アニメーションの列挙型。
	enum EnAnimationClip
	{
		enAnimationClip_Idle,
		enAnimationClip_Run,
		enAnimationClip_JumpStart,
		enAnimationClip_JumpAir,
		enAnimationClip_JumpEnd,
		enAnimationClip_Dive,
		enAnimationClip_Hit,
		enAnimationClip_Victory,
		enAnimationClip_Num
	};

	bool Start();
	Player();
	~Player();
	void Update();
	//移動処理。
	void Move(const Vector3&moveDir);
	//ジャンプ処理。
	void Jump();
	//回転処理。
	void Rotation(const Vector3& moveDir);
	//座標をリセットする。
	void ResetPosition();
	//コリジョン。
	void CheckCollision();
	//アニメーションを再生。
	void PlayAnimation();

	void Render(RenderContext& rc);
	
	//インデックスの設定。
	void SetControllerIndex(int index)
	{
		m_contRollerIndex = index;
	}

	//AIの設定。
	void SetIsAI(bool isAI)
	{
		m_isAI = isAI;
	}

	//AIか。
	bool IsAI() const
	{
		return m_isAI;
	}

	//キャラクターコントローラーの取得。
	CharacterController& GetCharacterController()
	{
		return m_characterController;
	}

	//ゴールの取得。
	void SetCanMove(bool canMove)
	{
		m_canMove = canMove;
	}

	//ゴールの設定。
	void SetGame(Game* game)
	{
		m_game = game;
	}

	//モデルのパスの設定。
	void SetModelPath(const std::string& path)
	{
		m_modelPath = path;
	}

	const char*GetModelPath() const
	{
		return m_modelPath.c_str();
	}

	//アニメーションの再生。
	void PlayerAnimationClip(int clipIndex, float blendTime);
	//アニメーションが終了したか。
	bool IsAnimationFinished() const;

	//アニメーションの再生時間を取得。
	float GetAnimationDuration(int clipIndex)
	{
		const auto& KeyFrames = m_animationClips[clipIndex].GetTopBoneKeyFrameList();
		if (KeyFrames.empty()) return 0.0f;
		return KeyFrames.back()->time;
	}
	
private:
	//メンバ変数。
	Game*					m_game=nullptr;									//ゲームのポインタ。
	Goal*					m_goal=nullptr;									//ゴールのポインタ。
	Actor*					m_actor=nullptr;								//アクターのポインタ。
	Hammer*					m_hammer = nullptr;								//ハンマーのポインタ。
	CharacterController		m_characterController;							//キャラクターコントローラー。
	FontRender				m_fontRender;									//フォントレンダー。
	AnimationClip			m_animationClips[enAnimationClip_Num];			//アニメーションクリップ。
	EnState					m_state=enState_Idle;							//ステート。
	Quaternion				m_currentRot;									//現在の回転。
	Vector3					m_forward=Vector3::Zero;						//前方向。
	Vector3					m_moveSpeed = Vector3::Zero;					//移動速度。
	Vector3					m_direction = Vector3::Zero;					//向き。
	Vector3					m_moveDir = Vector3::Zero;						//移動方向。
	Vector3					m_firstPosition = Vector3::Zero;				//初期位置。
	float					m_characterRadius=0.0f;							//キャラコンの半径。
	float					m_characterHeight=0.0f;							//キャラコンの高さ。
	float					m_stickMoveSpeed = 0.0f;						//スティックの移動速度。
	float					m_diveDuration=0.0f;							//ダイブの継続時間。
	float					m_diveTimer=0.0f;								//ダイブの時間。
	float					m_diveForwardSpeed=0.0f;						//ダイブの前進速度。
	float					m_diveRotationAngle=0.0f;						//ダイブの回転角度。
	float					m_fallGravity=0.0f;								//落下時の重力。
	float					m_blowPower = 0.0f;								//吹き飛ばし力。
	float					m_blowTimer = 0.0f;								//吹き飛ばす時間。
	float					m_airResistance = 0.0f;							//空気抵抗。
	float					m_animTimer = 0.0f;								//アニメーションの時間。
	float					m_animDuration = 0.0f;							//アニメーションの再生時間。
	bool					m_isJumping = false;							//ジャンプしているか。
	bool					m_isDiving = false;								//ダイブしているか。
	bool					m_isBlown = false;								//吹っ飛び中。
	bool					m_isJumpRequested = false;						//ジャンプができるか。
	bool					m_isAI = false;									//AIかどうか。
	bool					m_canMove = false;								//移動できるか。
	bool					m_isRespawn = false;							//フェード中かどうか。
	bool					m_isFalling = false;							//落下中かどうか。
	int						m_contRollerIndex = 0;							//コントローラーの数。
	int						m_currentAnimClip = -1;							//現在のアニメーションクリップ。
	std::string				m_modelPath;									//プレイヤーのモデルのパス。
	std::unique_ptr<PlayerControl>	m_playerControl;						//プレイヤーコントロール。
	std::unique_ptr<AIControl>		m_aiControl;							//AIコントロール。
};