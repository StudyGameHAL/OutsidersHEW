#include "CardState.h"
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

		if (cardAbilityFrameCount == 0)
		{
			// カードパワーを無効にする
			cardAbilityEnable[i] = false;
		}
	}
}

void CardState::OnEnterCard(CardPowerType cardPowerType)
{

}
