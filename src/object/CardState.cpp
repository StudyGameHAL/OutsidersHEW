#include "CardState.h"

void CardState::Initialize()
{
	for (int i = 0; i < static_cast<int>(CardAbilityType::COUNT); i++)
	{
		cardAbilityFrameCount[i] = 0;

		cardAbilityEnable[i] = false;
	}
}

void CardState::Update()
{
	for (int i = 0; i < static_cast<int>(CardAbilityType::COUNT); i++)
	{
		if (cardAbilityEnable[i] == false) continue;

		// カウントを減らす
		cardAbilityFrameCount[i]--;

		// カードパワーを実行する予定
		switch (static_cast<CardAbilityType>(i))
		{
		case CardState::CardAbilityType::TIMESTOP:
			// タイムストップ
			break;
		case CardState::CardAbilityType::SUPERATTACKPOWER:
			// 攻撃力増加
			break;
		default:
			break;
		}

		if (cardAbilityFrameCount[i] <= 0)
		{
			cardAbilityFrameCount[i] = 0;

			cardAbilityEnable[i] = false;
		}
	}
}


void CardState::OnEnterCard(CardAbilityType cardPowerType)
{
	for (int i = 0; i < static_cast<int>(CardAbilityType::COUNT); i++)
	{
		cardAbilityFrameCount[i] = cardAbilityFrameMax;

		cardAbilityEnable[i] = true;
	}
}
