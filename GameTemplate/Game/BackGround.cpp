#include "stdafx.h"
#include "BackGround.h"

bool BackGround::Start()
{
	//�����蔻���L���ɂ���B
	//PhysicsWorld::GetInstance()->EnableDrawDebugWireFrame();

	//�X�e�[�W�̓ǂݍ��݁B
	m_modelRender.Init("Assets/modelData/ground.tkm");

	//�ÓI�����I�u�W�F�N�g���쐬�B
	m_physicsStaticObject.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());

	return true;
}

BackGround::BackGround()
{

}

BackGround::~BackGround()
{
	
}


void BackGround::Update()
{
	//���f�����X�V�B
	m_modelRender.Update();
}


void BackGround::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}