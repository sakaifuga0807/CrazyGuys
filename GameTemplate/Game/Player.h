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

private:
	Quaternion m_rotation;//��]�B
	ModelRender m_modelRender;//���f�������_�[�B
	CharacterController m_characterController;//�L�����N�^�[�R���g���[���[�B
	Vector3 m_position;//���W�B
	Vector3 m_moveSpeed;//�ړ����x�B
	float m_characterRadius;//�L�����R���̔��a�B
	float m_characterHeight;//�L�����R���̍����B
	float m_stickMoveSpeed;//�X�e�B�b�N�̈ړ����x�B
	float m_gravity;//�d�́B
	float m_jumpPower;//�W�����v�́B
	float m_slideDuration;//�X���C�f�B���O�̎��ԁB
	float m_slideTimer;//�X���C�f�B���O�̃^�C�}�[�B
	float m_slideForwardSpeed;//�X���C�f�B���O�̑O�i���x�B
	bool m_isJumping = false;//�W�����v���Ă��邩�B
	bool m_isSliding = false;//�X���C�f�B���O���Ă��邩�B
};