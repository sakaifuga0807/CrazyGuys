#include "stdafx.h"
#include <fstream>
#include <iostream>
#include "External/nlohmann/json.hpp"
#include "GameCamera.h"
#include "Player.h"

using json = nlohmann::json;

bool GameCamera::Start()
{
	//GameCamera��Json�t�@�C����ǂݍ��ށB
	std::ifstream file{"Assets/config/GameCamera.json"};

	//�t�@�C�����J���Ȃ���΃X�L�b�v�B
	if (!file.is_open()) 
	{
		MessageBox(NULL,L"GameCamera��Json�t�@�C�����J���܂���B", L"�G���[", MB_OK);
		return false;
	}

	json cameraConfig;
	file >> cameraConfig;

	//Json�̒l�������o�ϐ��Ɋi�[�B
	auto gameCamera = cameraConfig["GameCamera"];

	//���W�������Ă���B
	auto pos = gameCamera["Position"];
	//�j�A�N���b�v�������Ă���B
	m_nearClip = gameCamera["NearClip"];
	//�t�@�[�N���b�v�������Ă���B
	m_farClip = gameCamera["FarClip"];
	//�����_�������Ă���B
	m_target_Y = gameCamera["CameraTargetOffSetY"];
	m_target_Z = gameCamera["CameraTargetOffSetZ"];

	//�����_���王�_�܂ł̃x�N�g����ݒ�B
	m_toCameraPos.Set(pos[0], pos[1], pos[2]);

	//�t�@�C�������B
	file.close();

	//�v���C���[�̃C���X�^���X��T���B
	m_player = FindGO<Player>("player");

	//�J�����̃j�A�N���b�v�ƃt�@�[�N���b�v��ݒ�B
	g_camera3D->SetNear(m_nearClip);
	g_camera3D->SetFar(m_farClip);

	return true;
}

GameCamera::GameCamera()
{

}

GameCamera::~GameCamera()
{

}

void GameCamera::Update()
{
	//�����_���v�Z����B
	Vector3 target = m_player->GetPosition();
	target.y += m_target_Y;
	target.z += m_target_Z;

	//�v���C���[�̒����_��ݒ�B
	Vector3 toCameraPosOld = m_toCameraPos;

	//�E�X�e�B�b�N���g���ăJ�������񂷁B
	float x = g_pad[0]->GetRStickXF();
	float y = g_pad[0]->GetRStickYF();

	//Y������̉�]�B
	Quaternion qRot;
	qRot.SetRotationDeg(Vector3::AxisY, 1.8f * x);
	qRot.Apply(m_toCameraPos);

	//X������̉�]�B
	Vector3 axisX;
	axisX.Cross(Vector3::AxisY, m_toCameraPos);
	axisX.Normalize();
	qRot.SetRotationDeg(axisX, 2.1f * y);
	qRot.Apply(m_toCameraPos);

	//�J�����̉�]�̏�����`�F�b�N����B
	Vector3 toPosDir = m_toCameraPos;
	toPosDir.Normalize();
	if (toPosDir.y > m_cameraYMAX)
	{
		//�J������������߂��B
		m_toCameraPos = toCameraPosOld;
	}
	if (toPosDir.y < m_cameraYMIN)
	{
		//�J�������������߂��B
		m_toCameraPos = toCameraPosOld;
	}

	//���_���v�Z����B
	Vector3 pos = target + m_toCameraPos;

	//���C���J�����ɒ����_�ƍ��W��ݒ�B
	g_camera3D->SetTarget(target);
	g_camera3D->SetPosition(pos);

	//�J�����̍X�V�B
	g_camera3D->Update();
}
