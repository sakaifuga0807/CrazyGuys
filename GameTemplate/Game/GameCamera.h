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
	Player* m_player;//プレイヤーのポインタ。
	Vector3 m_position=Vector3::Zero;//座標。
	Vector3 m_toCameraPos;//カメラ位置。
	float m_target_Y;//ターゲットY。
	float m_target_Z;//ターゲットZ。
	float m_nearClip;//ニアクリップ。
	float m_farClip;//ファークリップ。
	float m_cameraMax;//カメラのY座標の最大値。
	float m_cameraMin;//カメラのY座標の最小値。
	float m_rotationAngleY;//回転角度Y。
	float m_rotationAngleX;//回転角度X。
};

