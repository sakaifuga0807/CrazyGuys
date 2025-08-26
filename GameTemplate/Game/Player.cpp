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
	//�W�����v�͂������Ă���B
	m_jumpPower = playerData["JumpPower"];
	//�X���C�f�B���O���Ԃ������Ă���B
	m_slideDuration = playerData["SlideDuration"];
	//�X���C�f�B���O���̑S�g�X�s�[�h�������Ă���B
	m_slideForwardSpeed = playerData["SlideForwardSpeed"];

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


	//�ړ������B
	Move();
	//�W�����v�����B
	Jump();
	//��]�����B
	Rotation();
}

//�ړ������B
void Player::Move()
{
	//�X���C�f�B���O���������珈��������B
	if (m_isSliding)
	{
		m_slideTimer -= g_gameTime->GetFrameDeltaTime();

		//�X�e�B�b�N���͂�����Ȃ班���O�ɐi�ށB
		Vector3 stickL;
		stickL.x = g_pad[0]->GetLStickXF();
		stickL.y = g_pad[0]->GetLStickYF();

		if (stickL.Length() > 0.1f)
		{
			Vector3 forward = g_camera3D->GetForward();
			forward.y = 0.0f;
			forward.Normalize();

			m_moveSpeed.x = forward.x * m_slideForwardSpeed;
			m_moveSpeed.z = forward.z * m_slideForwardSpeed;
		}

		//�d�͂𔭐�������B
		m_moveSpeed.y -= m_gravity;

		//�t���[�����Ƃɍ��W���ړ�������B
		m_position = m_characterController.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());

		//�X���C�f�B���O���Ԃ��I�������X���C�f�B���O�I���B
		if (m_slideTimer <= 0.0f&&m_characterController.IsOnGround())
		{
			m_isSliding = false;
		}

		//���f���̍��W���Z�b�g����B
		m_modelRender.SetPosition(m_position);
		m_modelRender.Update();

		return;
	}

	//x��z�̈ړ����x���Ȃ����B
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
		//���n������W�����v�I���B
		m_isJumping = false;
	}
	else
	{
		//�d�͂𔭐�������B
		m_moveSpeed.y -= m_gravity;
	}

	//���f���̍��W���Z�b�g����B
	m_modelRender.SetPosition(m_position);
	m_modelRender.Update();
}

//�W�����v�����B
void Player::Jump()
{
	//�n�ʂɂ��Ă���΃W�����v�ł���B
	if (g_pad[0]->IsTrigger(enButtonA)&&m_characterController.IsOnGround())
	{
		m_isJumping = true;
		m_isSliding = false;
		m_moveSpeed.y += m_jumpPower;
		m_slideTimer = 0.0f;
	}
}

//��]�����B
void Player::Rotation()
{
	if (fabsf(m_moveSpeed.x) < 0.001f && fabsf(m_moveSpeed.z) < 0.001)
	{
		return;
	}

	if (m_isJumping)
	{
		//��]�p�x�B
		float angle = atan2(-m_moveSpeed.x, m_moveSpeed.z);

		m_rotation.SetRotationY(-angle);

		//��]��ݒ肷��B
		m_modelRender.SetRotation(m_rotation);
	}
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}