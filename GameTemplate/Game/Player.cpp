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

	///////////////////////////////////////////////////////////////////////////////
	//Json�t�@�C���̓ǂݍ��݁B
	///////////////////////////////////////////////////////////////////////////////
	
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
	m_diveDuration = playerData["DiveDuration"];
	//�X���C�f�B���O���̑S�g�X�s�[�h�������Ă���B
	m_diveForwardSpeed = playerData["DiveForwardSpeed"];
	//��]�p�x�������Ă���
	m_diveRotationAngle = playerData["RotationAngle"];
	//�������̏d�͔{���������Ă���B
	m_fallGravityScale = playerData["FallGravityScale"];

	//�t�@�C�������B
	file.close();
	///////////////////////////////////////////////////////////////////////////////
	//�I���B
	///////////////////////////////////////////////////////////////////////////////

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
	//���X�e�B�b�N�̓��͗ʂ��擾�B
	Vector3 stickL;
	stickL.x = g_pad[0]->GetLStickXF();
	stickL.y = g_pad[0]->GetLStickYF();	

	//�J�����̑O��ɂƉE�����̃x�N�g���������Ă���B
	Vector3 forward = g_camera3D->GetForward();
	Vector3 right = g_camera3D->GetRight();

	//y�����ɂ͈ړ������Ȃ��B
	forward.y = 0.0f; forward.Normalize();
	right.y = 0.0f; right.Normalize();

	//�_�C�u�łȂ���Έړ��������s���B
	if (!m_isDiving)
	{
		//x��z�̈ړ����x���Ȃ����B
		m_moveSpeed.x = 0.0f;
		m_moveSpeed.z = 0.0f;

		//XZ�����̈ړ����x���N���A�B
		m_moveSpeed += forward * stickL.y * m_stickMoveSpeed;
		m_moveSpeed += right * stickL.x * m_stickMoveSpeed;
	}
	else
	{
		//�X�e�B�b�N���|����Ă���΂��̕����Ɉړ��B
		if (stickL.Length() > 0.1)
		{
			Vector3 diveDir = forward * stickL.y + right * stickL.x;
			diveDir.Normalize();
			m_moveSpeed.x = diveDir.x * m_diveForwardSpeed;		//�������ւ̈ړ����x�����Z�B
			m_moveSpeed.z = diveDir.z * m_diveForwardSpeed;		//�E�����ւ̈ړ����x�����Z�B
		}
		else
		{
			//���X�Ɍ���������B
			m_moveSpeed.x *= 0.95;
			m_moveSpeed.z *= 0.95;
		}

		//�_�C�u���͑O�X�p���ɂ���B
		m_rotation.SetRotationX(ToRadian(m_diveRotationAngle));
		//��]��ݒ�B
		m_modelRender.SetRotation(m_rotation);
	}

	//�L�����R����Execute�֐��Ŗ��t���[���ړ�������
	m_position = m_characterController.Execute(m_moveSpeed, g_gameTime->GetFrameDeltaTime());

	//�L�����R�����n�ʂɕt���Ă���΁B
	if (m_characterController.IsOnGround())
	{
		//�d�͂��Ȃ����B
		m_moveSpeed.y = 0.0f;
		//�t���O��߂��B
		m_isJumping = false;
		m_isDiving = false;

		//��]�p�x���߂��B
		m_rotation.SetRotationX(0.0f);
		m_modelRender.SetRotation(m_rotation);
	}
	else
	{
		//�d�͂𔭐�������B
		m_moveSpeed.y -= m_gravity;
	}

	//���f���̍��W���Z�b�g����B
	m_modelRender.SetPosition(m_position);
	//���f�����X�V����B
	m_modelRender.Update();
}

//�W�����v�����B
void Player::Jump()
{
	//�n�ʂɂ��Ă���΃W�����v�ł���B
	if (g_pad[0]->IsTrigger(enButtonA)&&m_characterController.IsOnGround())
	{
		//�W�����v�̃t���O�𗧂Ă�B
		m_isJumping = true;

		m_isDiving = false;
		//�W�����v�͂�������B
		m_moveSpeed.y += m_jumpPower;
	}
	else if (m_isJumping&&!m_isDiving&&g_pad[0]->IsTrigger(enButtonA))
	{
		//�_�C�u�̃t���O�𗧂Ă�B
		m_isDiving = true;
		
		//�㏸���Ă���Ȃ犵���������Ĉ�C�ɗ���������B
		if (m_moveSpeed.y > 0)
		{
			m_moveSpeed.y = 0;
		}

		//�W�����v�����u�Ԃɂ�����Ƃ����O�ɐi�߂�B
		Vector3 forward = g_camera3D->GetForward();
		forward.y = 0.0f; forward.Normalize();


		m_moveSpeed.x = forward.x * m_diveForwardSpeed;
		m_moveSpeed.z = forward.z * m_diveForwardSpeed;
		m_moveSpeed.y -= m_gravity * m_fallGravityScale;	//����������������������B
	}
}

//��]�����B
void Player::Rotation()
{
	//�X�e�B�b�N�̓��͂��Ȃ���΃X�L�b�v�B
	if (fabsf(m_moveSpeed.x) < 0.001f && fabsf(m_moveSpeed.z) < 0.001)
	{
		return;
	}

	//��]�p�x�B
	float angle = atan2(-m_moveSpeed.x, m_moveSpeed.z);

	//���W�A����x�ɕϊ��B
	m_rotation.SetRotationY(-angle);

	//��]��ݒ肷��B
	m_modelRender.SetRotation(m_rotation);
}

void Player::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}