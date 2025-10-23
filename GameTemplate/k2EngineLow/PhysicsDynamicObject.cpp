/*!
* @brief	動的オブジェクト。
*/


#include "k2EngineLowPreCompile.h"
#include "PhysicsDynamicObject.h"

namespace nsK2EngineLow {
	PhysicsDynamicObject::PhysicsDynamicObject()
	{
	}
	PhysicsDynamicObject::~PhysicsDynamicObject()
	{
	}
	void PhysicsDynamicObject::CreateFromModel(Model& model, const Matrix& worldMatrix, const float restitution)
	{
		m_meshCollider.CreateFromModel(model, worldMatrix);
		RigidBodyInitData rbInfo;
		rbInfo.collider = &m_meshCollider;
		rbInfo.mass = 0.0f;
		rbInfo.restitution = restitution;
		m_rigidBody.Init(rbInfo);
	}
}
