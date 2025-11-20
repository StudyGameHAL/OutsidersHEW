#pragma once
#include "Pch.h"
#include "Transform.h"

class GameObject
{
public:
	GameObject() {};
	virtual ~GameObject() {};
	virtual void Update() {};
	virtual void Draw() {};

	const Transform& GetTransform() const { return m_Transform; }
	Transform& GetTransform() { return m_Transform; }
protected:
	Transform m_Transform;
};
