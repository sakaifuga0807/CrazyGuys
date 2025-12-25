#pragma once

class Player;

class GameCamera :public IGameObject
{
public:

	enum EnCameraMode
	{
		enCameraMode_Demo,		//デモモード。
		enCameraMode_Follow,	//追従モード。
	};

	bool Start();
	GameCamera();
	~GameCamera();
	void Update()override;
	void CameraMove();
	void UpdateDemoCamera();
	void StartFollow();


	//ターゲットを設定する関数。
	void SetTarget(Player* target)
	{
		m_player = target;
	}

	bool IsCameraDemoFinished() const
	{
		return m_demoTimer >= m_demoDuration;
	}

	bool IsDemoFinished() const
	{
		return m_demoTimer >= m_demoDuration;
	}

	//メンバ変数。
private:
	Player* m_player=nullptr;//プレイヤーのポインタ。
	Vector3 m_position=Vector3::Zero;//座標。
	Vector3 m_toCameraPos=Vector3::Zero;//カメラ位置。
	Vector3 m_demoStartPos = Vector3::Zero;//デモ開始位置。
	Vector3 m_demoEndPos = Vector3::Zero;//デモ終了位置。
	Vector3	m_demoTarget = Vector3::Zero;//デモターゲット。
	EnCameraMode m_cameraMode = enCameraMode_Demo;//カメラモード。
	float m_demoTimer = 0.0f;//デモタイマー。
	float m_demoDuration = 0.0f;//デモ時間。
	float m_target_Y=0.0f;//ターゲットY。
	float m_target_Z=0.0f;//ターゲットZ。
	float m_nearClip=0.0f;//ニアクリップ。
	float m_farClip=0.0f;//ファークリップ。
	float m_cameraMax=0.0f;//カメラのY座標の最大値。
	float m_cameraMin=0.0f;//カメラのY座標の最小値。
	float m_rotationAngleY=0.0f;//回転角度Y。
	float m_rotationAngleX=0.0f;//回転角度X。
public:
	bool m_enable = true;//カメラの有効無効。
};

