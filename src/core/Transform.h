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

public:
	Vector3 GetForwardVector() const
	{
		using namespace DirectX;

		XMVECTOR qRotation = XMQuaternionRotationRollPitchYaw(
			m_Rotation.x,
			m_Rotation.y,
			m_Rotation.z
		);

		XMVECTOR forwardLocal = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		XMVECTOR forwardRotated = XMVector3Rotate(forwardLocal, qRotation);

		Vector3 vec = Vector3(
			XMVectorGetX(forwardRotated),
			XMVectorGetY(forwardRotated),
			XMVectorGetZ(forwardRotated)
		);
		return vec;
	}

	Vector3 GetRightVector() const
	{
		using namespace DirectX;

		XMVECTOR qRotation = XMQuaternionRotationRollPitchYaw(
			m_Rotation.x,
			m_Rotation.y,
			m_Rotation.z
		);

		XMVECTOR rightLocal = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR rightRotated = XMVector3Rotate(rightLocal, qRotation);

		Vector3 vec = Vector3(
			XMVectorGetX(rightRotated),
			XMVectorGetY(rightRotated),
			XMVectorGetZ(rightRotated)
		);
		return vec;
	}

	Vector3 GetLeftVector() const
	{
		using namespace DirectX;

		XMVECTOR qRotation = XMQuaternionRotationRollPitchYaw(
			m_Rotation.x,
			m_Rotation.y,
			m_Rotation.z
		);

		XMVECTOR leftLocal = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR leftRotated = XMVector3Rotate(leftLocal, qRotation);

		Vector3 vec = Vector3(
			XMVectorGetX(leftRotated),
			XMVectorGetY(leftRotated),
			XMVectorGetZ(leftRotated)
		);
		return vec;
	}

private:
	Vector3	m_Position{};
	Vector3	m_Rotation{}; // ??
	Vector3 m_Scale{ 1.0f, 1.0f, 1.0f };
};