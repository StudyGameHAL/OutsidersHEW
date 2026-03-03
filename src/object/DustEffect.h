#pragma once

#include <random>
#include "GameObject.h"

class MODEL;
class DustEffect : public GameObject
{
private:
	enum class DustPhase
	{
		None,
		Expanding,
		Shrinking,
		Finished
	};
	static constexpr unsigned int EXPANDINGCOUNT = 12;
	static constexpr unsigned int SHRINKINGCOUNT = 26;

	static constexpr float resistance = 0.7f;

public:
	DustEffect() {};
	~DustEffect() {};

	void Initialize() override;
	void Update() override;
	void Draw() override;

	void SetVelocity(const Vector3& vel);
	void SetInitialScale(const Vector3& initialScale);
	void SetMaximumScale(const Vector3& maximumScale);

private:
	Vector3 m_InitialScale{1.0f,1.0f, 1.0f};
	Vector3 m_MaximumScale{3.0f, 3.0f, 3.0f};

	DustPhase dustPhase = DustPhase::None;

	Vector3 m_Velocity = { 0,0,0 };
	float m_RotationSpeed = 0;

	unsigned int m_PhaseCount = 0;

protected:
	MODEL* m_Model{};
};
