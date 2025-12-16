#include "TimeDelay.h"

#include <algorithm>


void TimeDelay::Initialize()
{
	m_TimeDelayEnable = false;
	SetTimeDelayPercentage(0);
}

void TimeDelay::Update()
{
	if (m_TimeDelayEnable == true)
	{
		// ’l‚ª‚Ù‚Æ‚ñ‚Ç“¯‚¶‚Æ‚«
		if (std::abs(m_TargetTimeDelayPercentage - m_CurrentTimeDelayPercentage) < Math::EPSILON)
		{
			m_CurrentTimeDelayPercentage = m_TargetTimeDelayPercentage;
			return;
		}

		// Percentage‚ðtarget‚É‹ß‚Ã‚¯‚éˆ—
		if (m_CurrentTimeDelayPercentage < m_TargetTimeDelayPercentage)
		{
			m_CurrentTimeDelayPercentage += speedPercentageMoveTowardsTarget;
		}
		else
		{
			m_CurrentTimeDelayPercentage -= speedPercentageMoveTowardsTarget;
		}
	}
	else
	{
		// ’l‚ª‚Ù‚Æ‚ñ‚Ç“¯‚¶‚Æ‚«
		if (std::abs(m_TargetTimeDelayPercentage - m_CurrentTimeDelayPercentage) < Math::EPSILON)
		{
			m_CurrentTimeDelayPercentage = m_TargetTimeDelayPercentage;
			return;
		}

		// Percentage‚ð0‚É‹ß‚Ã‚¯‚éˆ—
		if (m_CurrentTimeDelayPercentage < 0)
		{
			m_CurrentTimeDelayPercentage += speedPercentageMoveTowardsZero;
		}
		else
		{
			m_CurrentTimeDelayPercentage -= speedPercentageMoveTowardsZero;
		}
	}

}

void TimeDelay::EnableTimeDelay(float percentage)
{
	m_TimeDelayEnable = true;
	SetTimeDelayPercentage(percentage);
}

void TimeDelay::DisableTimeDelay()
{
	m_TimeDelayEnable = false;
	SetTimeDelayPercentage(0);
}

void TimeDelay::SetTimeDelayPercentage(int percentage)
{
	m_TargetTimeDelayPercentage = std::clamp(percentage, 0, 100);
}
