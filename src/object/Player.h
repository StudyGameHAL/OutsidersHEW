#pragma once
#include "object/GameObject.h"
#include "core/Vector.h"
#include "render/model.h"

enum class PlayerState
{
	IDLE,
	MOVING,
	DASHING,
	JAMPING,
	COUNT
};

class Player : public GameObject
{
private:

	PlayerState currentState = PlayerState::IDLE;

	Vector3 inputVelocity;

	Vector3 velocity;

	int health = 100;

	float jumpPower = 10.0f;

	float maxMoveSpeed = 20.0f;
	float moveSpeed = 20.0f;

	float friction = 10.0f;

	MODEL* currentModel;

	void HandleInput();
	void OnHitEnemy(class Enemy* enemy);

public:
	PlayerState GetState() const noexcept { return currentState; };
	void SetState(PlayerState state) noexcept { currentState = state; };

	float GetMaxMoveSpeed() const noexcept { return maxMoveSpeed; };
	void SetMaxMoveSpeed(float speed) noexcept { maxMoveSpeed = speed; };

	int GetHealth() const noexcept { return health; };
	void SetHealth(int _health) noexcept { health = _health; };

	float GetJumpPower() const noexcept { return jumpPower; };
	void SetJumoPower(float _jumpPower) noexcept { jumpPower = _jumpPower; };

	Vector3 GetVelocity() const noexcept { return velocity; };
	void SetVelocity(Vector3 _velocity) noexcept { velocity = _velocity; };

	void Update() override;

	void Draw() override;

	void Initialize() override;

	// ===== 衝突コールバックのオーバーライド =====
	bool OnCollision(GameObject* other, ColliderBase* myCollider,
	                ColliderBase* otherCollider, const OverlapResult& result) override;

	Player() = default;
	~Player() = default;
};

