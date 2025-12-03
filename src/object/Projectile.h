#pragma once

#include "GameObject.h"

class Projectile : public GameObject
{
public:
	Projectile();
	~Projectile();

	void Initialize() override;
	void Update() override;
	void Draw() override;

	// setter
	void SetVelocity(const Vector3& vel);
	void SetAttackPower(int power);

private:
	Vector3 m_Velocity = { 0,0,0 };
	int m_AttackPower = 1;
	float m_Lifetime = 2.0f;
	std::chrono::steady_clock::time_point m_old;



};