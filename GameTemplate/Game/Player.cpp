#include "stdafx.h"
#include <fstream>
#include "Player.h"
#include "External/nlohmann/json.hpp"
#include <iostream>
using json = nlohmann::json;

bool Player::Start()
{
	//���f����ǂݍ��ށB
	m_modelRender.Init("Assets/modelData/unityChan.tkm");

	//Player��json�t�@�C����ǂݍ��ށB
	std::ifstream file{ "Assets/config/Player.json" };

	//�t�@�C�����J���Ȃ��ꍇ�������X�L�b�v�B
	if (!file.is_open())
	{
		std::cerr << "json�t�@�C�����J���܂���ł����B" << std::endl;
		return false;
	}

	json configData;
	file >> configData;

	//Player�̃f�[�^���擾�B
	auto playerData = configData["Player"];

	//�X�e�B�b�N�̈ړ����x�������Ă���B
	m_stickMoveSpeed = playerData["StickMoveSpeed"];
	//�d�͂������Ă���B
	m_gravity = playerData["Gravity"];
	//�L�����R���̃T�C�Y�������Ă���B
	m_characterRadius = playerData["CharacterRadius"];
	m_characterHeight = playerData["CharacterHeight"];
	//���W�������Ă���B
	auto pos = playerData["Position"];
	//�W�����v�̈ړ����x�������Ă���B
	m_jump = playerData["Jump"];

	//���W���Z�b�g�B
	m_position.Set(pos[0], pos[1], pos[2]);

	//�L�����R�����������B
	m_characterController.Init(m_characterRadius, m_characterHeight, m_position);

	return true;
}

Player::Player()
{

}

Player::~Player()
{

}

void Player::Update()
{
	m_modelRender.Update();

	//�ړ�����
	Move();
	//�W�����v�����B
	Jump();
	//��]�����B
	Rotation();
}

void Player::Move()
{
	//x��z�̈ړ����x��0�ɂ���
	m_moveSpeed.x = 0.0f;
	m_moveSpeed.z = 0.0f;

	//���X�e�B�b�N�̓��͗ʂ��擾�B
	Vector3 stickL;
	stickL.x = g_pad[0]->GetLStickXF();
	stickL.y = g_pad[0]->GetLStickYF();

	//�J�����̑O��ɂƉE�����̃x�N�g���������Ă���B
	Vector3 forward = g_camera3D->GetForward();
	Vector3 right = g_camera3D->GetRight();

	//y�����ɂ͈ړ������Ȃ��B
	forward.y = 0.0f;
	forward.Normalize();
	right.y = 0.0f;
	right.Normalize();
	//XZ�����̈ړ����x���N���A�B
	m_moveSpeed += forward * stickL.y * m_stickMoveSpeed;	//�������ւ̈ړ����x�����Z�B
	m_moveSpeed += right * stickL.x * m_stickMoveSpeed;		//�E�����ւ̈ړ����x�����Z�B

	//�t���[�����Ƃɍ��W���ړ�������B
	m_position = m_characterController.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());

	//�L�����R�����n�ʂɓ������Ă��Ă�����B
	if (m_characterController.IsOnGround())
	{
		//�d�͂��Ȃ����B
		m_moveSpeed.y = 0.0f;
	}
	else
	{
		//�d�͂𔭐�������B
		m_moveSpeed.y -= m_gravity;
	}

	//���f���̍��W���Z�b�g����B
	m_modelRender.SetPosition(m_position);
}

void Player::Jump()
{
	//�n�ʂɂ��Ă���΃W�����v�ł���B
	if (g_pad[0]->IsTrigger(enButtonA)&&m_characterController.IsOnGround())
	{
		m_moveSpeed.y += m_jump;
	}
}

void Player::Rotation()
{
	if (fabsf(m_moveSpeed.x) < 0.001f && fabsf(m_moveSpeed.z) < 0.001)
	{
		return;
	}
	//��]�p�x�B
	float angle = atan2(-m_moveSpeed.x, m_moveSpeed.z);

	m_rotation.SetRotationY(-angle);

	//��]��ݒ肷��B
	m_modelRender.SetRotation(m_rotation);
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}