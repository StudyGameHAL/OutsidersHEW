#pragma once
#include "GameObject.h"


class Enemy : public GameObject
{
private:

private:
	Vector3 m_Velocity = Vector3(0.0f, 0.0f, 0.0f);
	int m_Health = 0;
	Vector3 m_MaxMoveSpeed = Vector3(0.0f, 0.0f, 0.0f);
	int m_AttackCount = 0;

private:
	static constexpr int m_CountToAttack = 120;
public:
	Enemy() {};
	virtual ~Enemy() {};

	void Update() override;
	void Draw() override;
};