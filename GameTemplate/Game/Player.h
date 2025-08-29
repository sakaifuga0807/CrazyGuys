#pragma once

class Player:public IGameObject
{
public:
	bool Start();
	Player();
	~Player();
	void Update();
	void Render(RenderContext& rc);
	//�ړ������B
	void Move();
	//�W�����v�����B
	void Jump();
	//��]�����B
	void Rotation();

	//���W���擾�B
	const Vector3& GetPosition()const
	{
		return m_position;
	}

	//���W��ݒ�B
	void SetPosition(Vector3 position)
	{
		m_position = position;
	}
	//���W�A���ɕϊ��B
	constexpr float ToRadian(float degree)
	{
		return degree * (3.14159265359f / 180.0f);
	}

	Vector3 m_position;//���W�B
private:
	Quaternion m_rotation;//��]�B
	ModelRender m_modelRender;//���f�������_�[�B
	CharacterController m_characterController;//�L�����N�^�[�R���g���[���[�B

	Vector3 m_moveSpeed;//�ړ����x�B
	float m_characterRadius;//�L�����R���̔��a�B
	float m_characterHeight;//�L�����R���̍����B
	float m_stickMoveSpeed;//�X�e�B�b�N�̈ړ����x�B
	float m_gravity;//�d�́B
	float m_jumpPower;//�W�����v�́B
	float m_diveDuration;//�_�C�u�̌p�����ԁB
	float m_diveTimer;//�_�C�u�̎��ԁB
	float m_diveForwardSpeed;//�_�C�u�̑O�i���x�B
	float m_diveRotationAngle;//�_�C�u�̉�]�p�x�B
	float m_fallGravityScale;//�������̏d�͔{���B
	bool m_isJumping = false;//�W�����v���Ă��邩�B
	bool m_isDiving = false;//�_�C�u���Ă��邩�B
};