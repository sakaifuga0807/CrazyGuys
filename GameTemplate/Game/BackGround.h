#pragma once

class BackGround:public IGameObject
{
public:
	//�����o�֐��B
	bool Start() ;
	BackGround();
	~BackGround();
	void Update();
	void Render(RenderContext& rc);

	//�����o�ϐ��B
private:
	ModelRender m_modelRender;//���f�������_�[�B
	PhysicsStaticObject m_physicsStaticObject;//�ÓI�����I�u�W�F�N�g�B
};

