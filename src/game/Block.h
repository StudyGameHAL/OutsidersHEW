#pragma once
#include "DynamicEntity.h"

// 動く可能性のあるブロックの例。
class Block : public DynamicEntity {
public:
    Block() = default;
    ~Block() override = default;

    // 必要であればエンティティの振る舞いをここに（未実装のまま）
    void Update(float /*dt*/) override {}
    void Draw() override {}
};
