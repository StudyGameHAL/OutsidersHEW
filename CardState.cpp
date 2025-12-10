#include "CardState.h"

void CardState::Initialize()
{
	for (int i = 0; i < static_cast<int>(CardPowerType::COUNT); i++)
	{
		CardPowerFrameCount[i] = 0;
		CardPowerEnable[i] = false;
	}
}

void CardState::Update()
{
	for (int i = 0; i < static_cast<int>(CardPowerType::COUNT); i++)
	{
		if (CardPowerEnable[i] == false) continue;

		// カウントを減らす
		CardPowerFrameCount[i]--;

		// カードパワーを実行する予定

		if (CardPowerFrameCount == 0)
		{
			// カードパワーを無効にする
			CardPowerEnable[i] = false;
		}
	}
}
