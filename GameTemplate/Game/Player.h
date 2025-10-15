#pragma once

#include "ICharacter.h"

class Goal;
class Result;
class Game;
class Actor;
class Hammer;

class Player:public ICharacter
{
public:
	bool Start();
	Player();
	~Player();
	void Update();
	//移動処理。
	void Move();
	//ジャンプ処理。
	void Jump();
	//回転処理。
	void Rotation();
	//座標をリセットする。
	void ResetPosition();
	//コリジョン。
	void CheckCollision();

	void Render(RenderContext& rc);

	//メンバ変数。
private:
	Game*					m_game=nullptr;						//ゲームのポインタ。
	Result*					m_result=nullptr;					//リザルトのポインタ。
	Goal*					m_goal=nullptr;						//ゴールのポインタ。
	Actor*					m_actor=nullptr;					//アクターのポインタ。
	Hammer*					m_hammer = nullptr;					//ハンマーのポインタ。
	CharacterController		m_characterController;				//キャラクターコントローラー。
	FontRender				m_fontRender;						//フォントレンダー。
	Quaternion				m_currentRot;						//現在の回転。
	Vector3					m_forward=Vector3::Zero;			//前方向。
	Vector3					m_moveSpeed = Vector3::Zero;		//移動速度。
	Vector3					m_direction = Vector3::Zero;		//向き。
	const Vector3			m_firstPosition = Vector3::Zero;	//初期位置。
	float					m_characterRadius=0.0f;				//キャラコンの半径。
	float					m_characterHeight=0.0f;				//キャラコンの高さ。
	float					m_stickMoveSpeed = 0.0f;			//スティックの移動速度。
	float					m_diveDuration=0.0f;				//ダイブの継続時間。
	float					m_diveTimer=0.0f;					//ダイブの時間。
	float					m_diveForwardSpeed=0.0f;			//ダイブの前進速度。
	float					m_diveRotationAngle=0.0f;			//ダイブの回転角度。
	float					m_fallGravityScale=0.0f;			//落下時の重力倍率。
	float					m_blowPower = 0.0f;					//吹き飛ばし力。
	float					m_blowTimer = 0.0f;					//吹き飛ばす時間。
	float					m_airResistance = 0.0f;				//空気抵抗。
	bool					m_isJumping = false;				//ジャンプしているか。
	bool					m_isDiving = false;					//ダイブしているか。
	bool					m_isBlown = false;					//吹っ飛び中。
};