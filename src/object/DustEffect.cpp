#include "DustEffect.h"
#include <iostream>
#include "render/model.h"
#include "render/Shader.h"
#include "Camera.h"


void DustEffect::Initialize()
{
	m_Model = ModelLoad("asset/model/Dust.fbx");

	static std::random_device rd;
	static std::mt19937 gen(rd());

	static std::uniform_real_distribution<float> angle_dist(-XM_2PI, XM_2PI);
	static std::uniform_real_distribution<float> speed_dist(-0.06f, 0.06f);
	m_RotationSpeed = speed_dist(gen);

	float randomAngle = angle_dist(gen);

	Vector3 rot = m_Transform.GetRotationEuler();
	m_Transform.SetRotationEuler({ rot.x, rot.y, rot.z + randomAngle });
}

void DustEffect::Update()
{
	constexpr float dt = 1.0f / 60.0f;
	
	auto pos = m_Transform.GetPosition();
	pos = pos + m_Velocity * dt;
	m_Transform.SetPosition(pos);

	Vector3 scale = m_Transform.GetScale();
	Vector3 rot = m_Transform.GetRotationEuler();
	Vector3 diff{};
	Vector3 rate{};

	rot.z += m_RotationSpeed; // Œˆ‚ß‚½‘¬“x‚ÅZŽ²‰ñ“]
	m_Transform.SetRotationEuler(rot);
	switch (dustPhase)
	{
	case DustEffect::DustPhase::None:
		dustPhase = DustEffect::DustPhase::Expanding;
		break;
	case DustEffect::DustPhase::Expanding:
		m_PhaseCount++;
		diff = m_MaximumScale - m_InitialScale;
		rate = { diff.x / EXPANDINGCOUNT , diff.y / EXPANDINGCOUNT , diff.z / EXPANDINGCOUNT };
		m_Transform.SetScale(scale + rate);
		if (m_PhaseCount > EXPANDINGCOUNT)
		{
			m_PhaseCount = 0;
			dustPhase = DustEffect::DustPhase::Shrinking;
		}
		break;
	case DustEffect::DustPhase::Shrinking:
		m_PhaseCount++;
		diff = m_MaximumScale;
		rate = { diff.x / SHRINKINGCOUNT , diff.y / SHRINKINGCOUNT , diff.z / SHRINKINGCOUNT };
		m_Transform.SetScale(scale - rate);
		if (m_PhaseCount > SHRINKINGCOUNT)
		{
			m_PhaseCount = 0;
			dustPhase = DustEffect::DustPhase::Finished;
		}
		break;
	case DustEffect::DustPhase::Finished:
		SetDeleted(true);
		break;
	default:
		break;
	}
	
}

void DustEffect::Draw()
{
	auto camera = GetSceneCamera();
	SHADER.begin();
	Matrix matrix{};
	matrix.projection = camera->GetProjectionMatrix();
	matrix.view = camera->GetViewMatrix();
	matrix.world = m_Transform.GetMatrix();
	SHADER.setMatrix(matrix);
	ModelDraw(m_Model);
}

void DustEffect::SetVelocity(const Vector3& vel)
{
	m_Velocity = vel;
}

void DustEffect::SetInitialScale(const Vector3& initialScale)
{
	m_InitialScale = initialScale;
	m_Transform.SetScale(m_InitialScale);
}

void DustEffect::SetMaximumScale(const Vector3& maximumScale)
{
	m_MaximumScale = maximumScale;
}
