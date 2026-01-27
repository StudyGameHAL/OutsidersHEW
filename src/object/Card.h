#pragma once

#include "GameObject.h"

class MODEL;
class CardSpawner;
class Card : public GameObject
{
public:
	Card() = default;
	~Card() = default;

	void Initialize() override;

	void Update() override;
	void Draw() override;

private:
	int m_PlayerSetAttackPower = 1;
	// 自分を作ったスポナー
	CardSpawner* m_Spawner;

public:
	// 自分を作ったスポナーを登録。
	void RegisterSpawner(CardSpawner* cardSpawner) { m_Spawner = cardSpawner; }

protected:
	MODEL* m_Model;

protected:
	// ===== 衝突コールバックのオーバーライド：Enemy専用ロジックを実装 =====
	bool OnCollision(GameObject* other, ColliderBase* myCollider,
		ColliderBase* otherCollider, const OverlapResult& result) override;
};
