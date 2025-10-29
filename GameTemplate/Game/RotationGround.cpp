#include "stdafx.h"
#include "RotationGround.h"
#include "Player.h"
#include "JsonUtility.h"


bool RotationGround::Start()
{
	m_modelRender.Init("Assets/objectData/RotationGround.tkm");

	m_physicsDynamicObj.CreateFromModel(m_modelRender.GetModel(), m_modelRender.GetModel().GetWorldMatrix());
	
	//À•W‚Æ‰ñ“]‚ğİ’èB
	m_modelRender.SetPosition(m_position);
	m_modelRender.SetRotation(m_rotation);
	m_physicsDynamicObj.SetPosition(m_position);
	m_physicsDynamicObj.SetRotation(m_rotation);
	
	return true;
}

RotationGround::RotationGround()
{

}

RotationGround::~RotationGround()
{

}

void RotationGround::Update()
{
	//Y²‚ğ’†S‚É‰ñ“]‚·‚é‰ñ“]—Ê‚ğì¬B
	Quaternion addRot;
	addRot.SetRotationDeg(Vector3(0.0f,1.0f,0.0f),m_angleSpeed*g_gameTime->GetFrameDeltaTime());
	//Œ»İ‚Ì‰ñ“]‚ÉŠ|‚¯‡‚í‚¹‚éB
	m_rotation.Multiply(addRot);
	//‰ñ“]‚ğİ’èB
	m_modelRender.SetRotation(m_rotation);
	m_physicsDynamicObj.SetRotation(m_rotation);

	m_modelRender.Update();
}

void RotationGround::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}