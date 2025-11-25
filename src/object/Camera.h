#pragma once
#include <DirectXMath.h>
#include "GameObject.h"
#include "core/Vector.h"

class Camera : public GameObject
{
public:
	Camera() = default;
	virtual ~Camera() = default;
	DirectX::XMMATRIX GetViewMatrix() const;
	DirectX::XMMATRIX GetProjectionMatrix() const;
	Vector3 GetUp() const { return m_Up; }
	Vector3 GetTarget() const { return m_Target; }
	void SetUp(Vector3 up) { m_Up = up; }
	void SetTarget(Vector3 target) { m_Target = target; }
protected:
	Vector3 m_Target{};
	Vector3 m_Up{ 0.0f, 1.0f, 0.0f };
	float m_Fov{ 1.0f };
	float m_ZNear{ 1.0f };
	float m_ZFar{ 1000.0f };
};
