#pragma once

#include "src/object/GameObject.h"

class CardState : public GameObject
{
public:
	// カードの力一覧
	enum class CardPowerType : int
	{
		TIMESTOP,
		SUPERATTACKPOWER,
		COUNT
	};

protected:
	// カードのPowerTypeとFrameCountのペア変数
	int CardPowerFrameCount[static_cast<int>(CardPowerType::COUNT)];
	
	bool CardPowerEnable[static_cast<int>(CardPowerType::COUNT)];
public:
	CardState() = default;
	~CardState() = default;

	void Initialize() override;
	void Update() override;
	void Draw() {};
};