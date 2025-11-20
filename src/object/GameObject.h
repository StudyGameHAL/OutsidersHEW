#pragma once
#include "Pch.h"
#include "core/Transform.h"

class GameObject
{
public:
	GameObject() {};
	virtual ~GameObject() {};
	virtual void Initialize() {};
	virtual void Update() {};
	virtual void Draw() {};
	virtual void Finalize() {}
	const Transform& GetTransform() const { return m_Transform; }
	Transform& GetTransform() { return m_Transform; }
protected:
	Transform m_Transform;
};
