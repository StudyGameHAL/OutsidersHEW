#pragma once
#include "Projectile.h"

class NormalCard : public Projectile
{
public:
	NormalCard() = default;
	~NormalCard() = default;

	void Initialize() override;

	void Update() override;

protected:
	// ===== 衝突コールバックのオーバーライド：Enemy専用ロジックを実装 =====
	bool OnCollision(GameObject* other, ColliderBase* myCollider,
		ColliderBase* otherCollider, const OverlapResult& result) override;
};