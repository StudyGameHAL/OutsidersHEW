#pragma once

#include "src/object/GameObject.h"

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

protected:
	// カードのPowerTypeとFrameCountのペア変数
	int cardAbilityFrameCount[static_cast<int>(CardAbilityType::COUNT)];
	
	bool cardAbilityEnable[static_cast<int>(CardAbilityType::COUNT)];
public:
	CardState() = default;
	~CardState() = default;

	void Initialize() override;
	void Update() override;
	void Draw() {};

	void OnEnterCard(CardAbilityType newCardAbilityType);
};