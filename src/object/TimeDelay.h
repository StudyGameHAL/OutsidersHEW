#pragma once
#include "GameObject.h"

class TimeDelay : public GameObject
{
private:
	bool m_TimeDelayEnable = false;
	// 0~100‚Ì”ÍˆÍ‚Åİ’è  100‚ÅŠ®‘S’â~
	float m_TargetTimeDelayPercentage = 0;
	float m_CurrentTimeDelayPercentage = 0;

	// currentPercentage‚ªtargetPercentage‚ÉŒü‚©‚¤‚Æ‚«‚Ì‘¬“x
	static constexpr float speedPercentageMoveTowardsTarget = 1.0f;
	// currentPercentage‚ªZero‚ÉŒü‚©‚¤‚Æ‚«‚Ì‘¬“x
	static constexpr float speedPercentageMoveTowardsZero = 1.0f;
public:
	void Initialize() override;
	void Update() override;

	void EnableTimeDelay(float percentage);
	void DisableTimeDelay();
	bool GetTimeDelayEnabledState() { return m_TimeDelayEnable; }

	void SetTimeDelayPercentage(int percentage);
	int GetTimeDelayPercentage() { return m_TargetTimeDelayPercentage; }
};