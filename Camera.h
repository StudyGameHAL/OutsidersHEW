#pragma once
#include <DirectXMath.h>
#include "GameObject.h"
#include "Vector.h"

using namespace DirectX;

class Camera : public GameObject
{
public:
	Camera() = default;
	virtual ~Camera() = default;

	virtual void Update() {}
	virtual void PreDraw() {}
	virtual void Draw() {}
	virtual void DrawDebug() {}

	XMFLOAT4X4 GetViewMatrix() { return m_ViewMatrix; }
	XMFLOAT4X4 GetProjectionMatrix() { return m_ProjectionMatrix; }

	void SetUp(Vector3 up) { m_Up = up; }
	void SetTarget(Vector3 target) { m_Target = target; }
	Vector3 GetUp() { return m_Up; }
	Vector3 GetTarget() { return m_Target; }
protected:
	Vector3		m_Target;
	Vector3		m_Up;
	XMFLOAT4X4	m_ViewMatrix;
	XMFLOAT4X4	m_ProjectionMatrix;
};
