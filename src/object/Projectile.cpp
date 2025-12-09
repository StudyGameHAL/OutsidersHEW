#include "Projectile.h"
#include "render/model.h"
#include "render/Shader.h"
#include "Camera.h"
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
	m_old = std::chrono::steady_clock::now();
	m_model = ModelLoad("asset/model/ball.fbx");

}

void Projectile::Update()
{
	auto now = std::chrono::steady_clock::now();
	std::chrono::duration<float> delta = now - m_old;
	m_old = now;

	float dt = delta.count();

	// Move projectile
	auto pos = m_Transform.GetPosition();
	pos = pos + m_Velocity * dt;
	m_Transform.SetPosition(pos);

	// Reduce lifetime
	m_Lifetime -= dt;
	if (m_Lifetime <= 0.0f)
	{
		// destroy ?
	}
}

void Projectile::Draw()
{
	auto camera = GetSceneCamera();
	SHADER.begin();
	Matrix matrix{};
	matrix.projection = camera->GetProjectionMatrix();
	matrix.view = camera->GetViewMatrix();
	matrix.world = m_Transform.GetMatrix();
	SHADER.setMatrix(matrix);
	ModelDraw(m_model);
}

void Projectile::SetVelocity(const Vector3& vel)
{
	m_Velocity = vel;
}

void Projectile::SetAttackPower(int power)
{
	m_AttackPower = power;
}
