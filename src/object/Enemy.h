#pragma once
#include "GameObject.h"
#include "render/model.h"

class Enemy : public GameObject
{
private:

private:
	Vector3 m_Velocity = Vector3(0.0f, 0.0f, 0.0f);
	int m_Health = 5;
	Vector3 m_MaxMoveSpeed = Vector3(0.0f, 0.0f, 0.0f);
	int m_AttackCount = 0;

	MODEL* m_Model;

private:
	static constexpr int m_CountToAttack = 25;
public:
	Enemy() = default;
	virtual ~Enemy() = default;

	void Initialize() override;

	void Update() override;
	void Draw() override;

	void ReciveDamage(int damage) noexcept { m_Health -= damage; }

protected:
	void ApplyRotationToPlayer();

	// ===== 衝突コールバックのオーバーライド：Enemy専用ロジックを実装 =====
	bool OnCollision(GameObject* other, ColliderBase* myCollider,
		ColliderBase* otherCollider, const OverlapResult& result) override;

};