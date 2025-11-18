#pragma once
#include "Entity.h"
#include "../core/Rigidbody.h"

// 物理的に動く可能性があるエンティティ。
class DynamicEntity : public Entity {
public:
    DynamicEntity() = default;
    ~DynamicEntity() override = default;

    Rigidbody rigidbody{}; // 追加データ（制御は未実装）
};
