#pragma once

class Player:public IGameObject
{
public:
	bool Start();
	Player();
	~Player();
	void Update();
	void Render(RenderContext& rc);
	//移動処理。
	void Move();
	//ジャンプ処理。
	void Jump();
	//回転処理。
	void Rotation();

	//座標を取得。
	const Vector3& GetPosition()const
	{
		return m_position;
	}

	//座標を設定。
	void SetPosition(Vector3 position)
	{
		m_position = position;
	}
	//ラジアンに変換。
	constexpr float ToRadian(float degree)
	{
		return degree * (3.14159265359f / 180.0f);
	}

	Vector3 m_position;//座標。
private:
	Quaternion m_rotation;//回転。
	ModelRender m_modelRender;//モデルレンダー。
	CharacterController m_characterController;//キャラクターコントローラー。

	Vector3 m_moveSpeed;//移動速度。
	float m_characterRadius;//キャラコンの半径。
	float m_characterHeight;//キャラコンの高さ。
	float m_stickMoveSpeed;//スティックの移動速度。
	float m_gravity;//重力。
	float m_jumpPower;//ジャンプ力。
	float m_diveDuration;//ダイブの継続時間。
	float m_diveTimer;//ダイブの時間。
	float m_diveForwardSpeed;//ダイブの前進速度。
	float m_diveRotationAngle;//ダイブの回転角度。
	float m_fallGravityScale;//落下時の重力倍率。
	bool m_isJumping = false;//ジャンプしているか。
	bool m_isDiving = false;//ダイブしているか。
};