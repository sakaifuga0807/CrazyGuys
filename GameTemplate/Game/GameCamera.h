#pragma once

class Player;

class GameCamera :public IGameObject
{
public:
	bool Start();
	GameCamera();
	~GameCamera();
	void Update();

private:
	Player* m_player;//�v���C���[�̃|�C���^�B
	//Vector3 m_position=Vector3::Zero;//���W�B
	Vector3 m_toCameraPos;//�J�����ʒu�B
	float m_target_Y=0.0f;//�^�[�Q�b�gY�B
	float m_target_Z=0.0f;//�^�[�Q�b�gZ�B
	float m_nearClip=0.0f;//�j�A�N���b�v�B
	float m_farClip=0.0f;//�t�@�[�N���b�v�B
	float m_cameraYMAX=0.0f;//�J������Y���W�̍ő�l�B
	float m_cameraYMIN=0.0f;//�J������Y���W�̍ŏ��l�B
};

