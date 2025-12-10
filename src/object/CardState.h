#pragma once

#include "GameObject.h"

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
	// カードの能力のフレームを保存する変数
	int CardPowerFrameCount[static_cast<int>(CardPowerType::COUNT)];
	// カードの能力が有効かどうか
	bool CardPowerEnable[static_cast<int>(CardPowerType::COUNT)];
public:
	CardState() = default;
	~CardState() = default;

	void Initialize() override;
	void Update() override;
	void Draw() {};
};