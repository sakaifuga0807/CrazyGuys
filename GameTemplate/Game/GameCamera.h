#pragma once

class Player;

class GameCamera :public IGameObject
{
public:
	bool Start();
	GameCamera();
	~GameCamera();
	void Update();

	void CameraMove();

	//メンバ変数。
private:
	Player* m_player=nullptr;//プレイヤーのポインタ。
	Vector3 m_position=Vector3::Zero;//座標。
	Vector3 m_toCameraPos=Vector3::Zero;//カメラ位置。
	float m_target_Y=0.0f;//ターゲットY。
	float m_target_Z=0.0f;//ターゲットZ。
	float m_nearClip=0.0f;//ニアクリップ。
	float m_farClip=0.0f;//ファークリップ。
	float m_cameraMax=0.0f;//カメラのY座標の最大値。
	float m_cameraMin=0.0f;//カメラのY座標の最小値。
	float m_rotationAngleY=0.0f;//回転角度Y。
	float m_rotationAngleX=0.0f;//回転角度X。
};

