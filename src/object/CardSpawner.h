#pragma once

#include "GameObject.h"

class MODEL;
class Card;
class CardSpawner : public GameObject
{
public:
	CardSpawner() = default;
	~CardSpawner() = default;

	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	void SpawnCard();

public:
	// 作ったCardが消えたときの処理。
	void OnCardDeleted() { cardExists = false; }

private:
	static constexpr int coolCountMax = 180;
	int coolCount = 0;
	bool cardExists = false;

protected:
	MODEL* m_Model = nullptr;
};

/*
カードをスポーンする　カードが壊れる　クールダウンをカウントする　カードをスポーンする
*/