#pragma once

#include "GameObject.h"

class CardState : public GameObject
{
public:
	// カードの力一覧
	enum class CardAbilityType : int
	{
		TIMESTOP,
		SUPERATTACKPOWER,
		COUNT
	};

private:
	static constexpr int cardAbilityFrameMax = 240;

private:
	// カードの能力のフレームを保存する変数
	int cardAbilityFrameCount[static_cast<int>(CardAbilityType::COUNT)];
	// カードの能力が有効かどうか
	bool cardAbilityEnable[static_cast<int>(CardAbilityType::COUNT)];

public:
	CardState() = default;
	~CardState() = default;

	void Initialize() override;
	void Update() override;
	void Draw() {};

	void OnEnterCard(CardAbilityType cardAbilityType);
};