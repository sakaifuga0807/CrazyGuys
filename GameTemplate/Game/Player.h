#pragma once

#include "ICharacter.h"

class Goal;
class Result;
class Game;
class Actor;

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
	void Collision();

	void Render(RenderContext& rc);

	//ラジアン角に変換。
	constexpr float ToRadian(float degree)
	{
		return degree * (3.14159265359f / 180.0f);
	}

	//メンバ変数。
private:
	Game* m_game;//ゲームのポインタ。
	Result* m_result;//リザルトのポインタ。
	Goal* m_goal;//ゴールのポインタ。
	Actor* m_actor;//アクターのポインタ。
	ModelRender m_modelRender;//モデルレンダー。
	CharacterController m_characterController;//キャラクターコントローラー。
	FontRender m_fontRender;//フォントレンダー。
	Quaternion m_rotation;//回転。
	Quaternion m_currentRot;//現在の回転。
	Vector3 m_moveSpeed=Vector3::Zero;//移動速度。
	Vector3 m_forward=Vector3::Zero;//前方向。
	const Vector3 m_firstPosition = Vector3::Zero;//初期位置。
	float m_characterRadius=0.0f;//キャラコンの半径。
	float m_characterHeight=0.0f;//キャラコンの高さ。
	float m_stickMoveSpeed = 0.0f;;//スティックの移動速度。
	//float m_gravity=0.0f;//重力。
	//float m_jumpPower=0.0f;//ジャンプ力。
	float m_diveDuration=0.0f;//ダイブの継続時間。
	float m_diveTimer=0.0f;//ダイブの時間。
	float m_diveForwardSpeed=0.0f;//ダイブの前進速度。
	float m_diveRotationAngle=0.0f;//ダイブの回転角度。
	float m_fallGravityScale=0.0f;//落下時の重力倍率。
	bool m_isJumping = false;//ジャンプしているか。
	bool m_isDiving = false;//ダイブしているか。
public:
	//Vector3 m_position=Vector3::Zero;//座標。
};