#pragma once
#include "Vector.h"

class Transform
{
public:
	const Vector3& GetPosition() const { return m_Position; }
	const Vector3 GetRotationEuler() const { return m_Rotation; }
	const Vector3& GetScale() const { return m_Scale; }

	void SetPosition(const Vector3& pos) { m_Position = pos; }
	void SetRotationEuler(const Vector3& rot) { m_Rotation = rot; }
	void SetScale(const Vector3& scale) { m_Scale = scale; }

	DirectX::XMMATRIX GetMatrix() const
	{
		using namespace DirectX;
		XMVECTOR qRotation = XMQuaternionRotationRollPitchYaw(
			m_Rotation.x,
			m_Rotation.y,
			m_Rotation.z
		);
		DirectX::XMMATRIX mat = DirectX::XMMatrixAffineTransformation(
			m_Scale.ToXMVECTOR(),
			DirectX::XMVectorZero(), // rotation origin
			qRotation, // quaternion
			m_Position.ToXMVECTOR() // translation
		);

		return mat;
	}
private:
	Vector3	m_Position{};
	Vector3	m_Rotation{}; // ??
	Vector3 m_Scale{ 1.0f, 1.0f, 1.0f };
};