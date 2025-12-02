#include "Projectile.h"
#include <chrono>
#include "Game.h"
Projectile::Projectile()
{
}

Projectile::~Projectile()
{
}

void Projectile::Initialize()
{
	m_Lifetime = 2.0f;
}

void Projectile::Update()
{
	static auto old = std::chrono::steady_clock::now();
	auto now = std::chrono::steady_clock::now();

	std::chrono::duration<float> delta = now - old;
	old = now;

	float dt = delta.count();

	// Move projectile
	auto pos = m_Transform.GetPosition();
	pos = pos + m_Velocity * dt;

	// Reduce lifetime
	m_Lifetime -= dt;
	if (m_Lifetime <= 0.0f)
	{
		// destroy ?
	}
}

void Projectile::Draw()
{
}

void Projectile::SetVelocity(const Vector3& vel)
{
	m_Velocity = vel;
}

void Projectile::SetAttackPower(int power)
{
	m_AttackPower = power;
}
