#pragma once
#include "GameObject.h"
#include "render/model.h"

class Enemy : public GameObject
{
private:

private:
	Vector3 m_Velocity = Vector3(0.0f, 0.0f, 0.0f);
	int m_Health = 0;
	Vector3 m_MaxMoveSpeed = Vector3(0.0f, 0.0f, 0.0f);
	int m_AttackCount = 0;

	MODEL* m_Model;

private:
	static constexpr int m_CountToAttack = 120;
public:
	Enemy() = default;
	virtual ~Enemy() = default;

	void Initialize() override;

	void Update() override;
	void Draw() override;

protected:
	void ApplyRotationToPlayer();

};	