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
public:
	enum class CardAbilityType : int
	{
		TIMESTOP,
		SUPERATTACKPOWER,
		COUNT
	};

private:
	static constexpr int cardAbilityFrameMax = 120;

private:

	PlayerState currentState = PlayerState::IDLE;

	Vector3 inputVelocity;

	Vector3 velocity;

	int health = 100;
	int damage = 10;
	bool isInvincible = false;
	float invincibleTimer = 0.0f;
	float invincibleDuration = 1.0f;

	float jumpPower = 10.0f;
	
	float maxMoveSpeed = 20.0f;
	float moveSpeed = 20.0f;

	float friction = 10.0f;

	MODEL* currentModel = nullptr;

	void HandleInput();

private:

	// カードの能力のフレームを保存する変数
	int cardAbilityFrameCount[static_cast<int>(CardAbilityType::COUNT)];
	// カードの能力が有効かどうか
	bool cardAbilityEnable[static_cast<int>(CardAbilityType::COUNT)];

public:
	PlayerState GetState() const noexcept { return currentState; };
	void SetState(PlayerState state) noexcept { currentState = state; };

	float GetMaxMoveSpeed() const noexcept { return maxMoveSpeed; };
	void SetMaxMoveSpeed(float speed) noexcept { maxMoveSpeed = speed; };

	int GetHealth() const noexcept { return health; };
	void SetHealth(int _health) noexcept { health = _health; };

	float GetJumpPower() const noexcept { return jumpPower; };
	void SetJumpPower(float _jumpPower) noexcept { jumpPower = _jumpPower; };

	Vector3 GetVelocity() const noexcept { return velocity; };
	void SetVelocity(Vector3 _velocity) noexcept { velocity = _velocity; };

	void Update() override;
	void NoTimeStopUpdate() override;

	void Draw() override;

	void Initialize() override;

	// ===== 衝突コールバックのオーバーライド =====
	bool OnCollision(GameObject* other, ColliderBase* myCollider,
		ColliderBase* otherCollider, const OverlapResult& result) override;

	Player() = default;
	~Player() = default;

private:
	void CardStateUpdate();
	// ===== カードに当たった時の処理 =====
	void EnableCard(Player::CardAbilityType cardAbilityType);
	// ===== カードの効果が終了した時の処理 =====
	void DisableCard(Player::CardAbilityType cardAbilityType);
};

